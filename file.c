#include "raylib.h"
#include "testje" 
#include "./interface.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

Vector2 getFullMousePos()
{
  int mx = GetMouseX() + GetWindowPosition().x;
  int my = GetMouseY() + GetWindowPosition().y;
  return (Vector2){mx, my};
}


float nLerp(float current, float target, float amount)
{
  if (current < target)
  {
    current += amount*(target - current);
    if (current > target) { return target; }
    return current;
  }
  else
  {
    current -= amount*(current - target);
    if (current < target) { return target; } 
    return current;
  }
}


float approach(float current, float target, float amount)
{
  if (current < target)
  {
    current += amount; 
    if (current > target) { return target; } 
  } 
  else
  {
    current -= amount; 
    if (current < target) { return target; } 
  } 
  return current;
}


int main(int argc, char *argv[])
{
  // Initialization
  //--------------------------------------------------------------------------------------
  int screenWidth = 1000;
  int screenHeight = 600;
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Editor - wip");
  SetWindowState(FLAG_WINDOW_UNDECORATED);
  SetWindowPosition(500,200);
  SetTargetFPS(100);               // Set our game to run at 60 frames-per-second
  SetExitKey(KEY_LEFT_ALT);

  RenderTexture2D tex;
  RenderTexture2D tex2;
  tex = LoadRenderTexture(screenWidth, screenHeight);
  tex2 = LoadRenderTexture(screenWidth, screenHeight);
  while(!IsRenderTextureReady(tex));
  int textSize = 20;
  int hlTextSize = 15;
  char *fontname_std = "C:/Users/Gebruiker/Desktop/nraytest/editor/resources/JetBrainsMono-Bold.ttf";
  Font myFont = LoadFontEx(fontname_std, textSize, 0, 0);
  Font highlightFont = LoadFontEx(fontname_std, hlTextSize, 0, 0);
  SetTextureFilter(myFont.texture, TEXTURE_FILTER_TRILINEAR);
  SetTextureFilter(highlightFont.texture, TEXTURE_FILTER_TRILINEAR);
  
  int lineHeight = (int)(myFont.recs[0].height);
  int glphWidth = (int)(myFont.recs[0].width);
  int hlGlphWidth = (int)(highlightFont.recs[0].width);

  // settings

  Color edgeLight = (Color){200,200,200,200};
  Color edgeDark = (Color){0,0,0,100};
  Color cursorColor = { 20, 20, 250, 200 };
  Color cursorInnerColor = { 20, 20, 20, 10 };
  Color textCol = {20, 20, 50, 255};
  Color shadowTextCol = {textCol.r, textCol.g, textCol.b, 60};
  Color lineNumCol = {textCol.r, textCol.g, textCol.b, 180};
  Color grad1 = {130,130,130, 255};
  Color grad2 = {155, 138, 118, 255};
  Color grad3 = {182, 147, 105, 255};

  int speedTrigger = 20;
  int delay = 3;
  float lerptime = 0.1;
  int resizeMargin = 20;
  int fileSelection = 0;
  DIR *currentDir;
  int currentDirSize=0;
  int lowerHinge = 5;
  int upperHinge = 15;

  //--------------------------------------------------------------------------------------

  Texture2D buttonCollection[4];
  createButtons(buttonCollection, "resources/Bar.png", "resources/Groupcog.png",  "resources/Vectorminus.png",  "resources/Vectorcross.png");
  Texture2D bar_t = buttonCollection[0];
  Texture2D cog_t = buttonCollection[1];
  Texture2D minus_t = buttonCollection[2];
  Texture2D cross_t = buttonCollection[3];

  Shader shader;
  Shader shaderBlur;

  Vector2 textCursor = {0,0};
  int closeAtEndOfLoop = 0;
  int minimizeAtEndOfLoop = 0;
  bool isResizing = false;
  int scrollOfset = 0;
  int selectOffset = 0;
  int oldScrollOffset = 0;
  int oldSelectOffset = 0;
  int oldSelection = 0;
  int frameCounter = 0;
  float lerpval = 0;
  bool lerping = false;
  int shouldBlur = 0;
  bool cogging = 0;
  bool peekMode = false;
  int uph;
  int downh;
  float closeLerpVal = 0;
  float minLerpVal = 0;
  float minOpa = 1;
  int peekOffset = 0;
  int peekFileLength = 0;

  struct stat fs;
  char * fileList[500];
  for (int i = 0; i<500; i++)
  {
    fileList[i] = malloc(sizeof(char) * 500);    
    *fileList[i] = 0;
  }

  currentDir = opendir(".");
  struct dirent *entry;
  int fileCounter = 0;
  while ((entry = readdir(currentDir)) != NULL)
  {
    printf("%s\n", entry->d_name);
    strcpy(fileList[fileCounter], entry->d_name);
    fileCounter++;
  }
  currentDirSize = fileCounter;

  // Main game loop

  while (!WindowShouldClose())    // Detect window close button or ESC key
  {

    SetWindowSize(screenWidth, screenHeight);
    int oncog = 0;
    int onminus = 0;
    int oncross = 0;
    frameCounter++;
    float time = GetTime();

    if (!cogging) {

      if (IsKeyPressed(KEY_EQUAL)) 
      {
        if (textSize < 100)
        {
          textSize += 1;
          hlTextSize += 1;
          UnloadFont(myFont);
          UnloadFont(highlightFont);
          myFont = LoadFontEx(fontname_std, textSize, 0, 0);
          highlightFont = LoadFontEx(fontname_std, hlTextSize, 0, 0);

          lineHeight = (int)( myFont.recs[0].height);
          glphWidth = (int)(myFont.recs[0].width);
          hlGlphWidth = (int)(highlightFont.recs[0].width);
        }
      }

      if (IsKeyPressed(KEY_MINUS))
      {
        if (textSize > 8)
        {
          textSize -= 1;
          hlTextSize -= 1;
          UnloadFont(myFont);
          UnloadFont(highlightFont);
          myFont = LoadFontEx(fontname_std, textSize, 0, 0);
          highlightFont = LoadFontEx(fontname_std, hlTextSize, 0, 0);

          lineHeight = (int)(myFont.recs[0].height);
          glphWidth = (int)(myFont.recs[0].width);
          hlGlphWidth = (int)(highlightFont.recs[0].width);
        }
      }
      
      // file actions
      if (IsKeyPressed(KEY_SPACE))
      {
        peekOffset = 0;
        peekMode = !peekMode;
      }

      if (IsKeyPressed(KEY_J))
      {
        if (!peekMode)
        {
          scrollOfset--;
          fileSelection++;
          if (fileSelection > currentDirSize-1) 
          {
            fileSelection = 0;
            scrollOfset = 0;
          }
        }
      }

      if (IsKeyPressed(KEY_K))
      {
        if (!peekMode)
        {
          scrollOfset++;
          fileSelection--;
          if (fileSelection < 0) 
          {
            fileSelection = currentDirSize-1;
            scrollOfset = -(currentDirSize-1);
          }
        }
      }
      
      if (IsKeyDown(KEY_K))
      {
        if (peekOffset > 0 && peekMode) peekOffset -= 1;  
      }

      if (IsKeyDown(KEY_J))
      {
        if (peekOffset < peekFileLength - (screenHeight / lineHeight) + 2 && peekMode) peekOffset += 1;  
      }
      
      
      if (IsKeyPressed(KEY_L))
      {
        int r = stat(fileList[fileSelection], &fs);
        if ( r==-1) {puts("error reading filename with stat"); exit(1);}
        if (S_ISDIR(fs.st_mode))
        {
          closedir(currentDir);
          currentDir = opendir(fileList[fileSelection]);
          chdir(fileList[fileSelection]);
          oldSelection = fileSelection;
          fileSelection = 0;
          for (int i = 0; i<500; i++)
          {
            *fileList[i] = 0;
          }
          struct dirent *entry;
          int fileCounter = 0;
          while ((entry = readdir(currentDir)) != NULL)
          {
            printf("%s\n", entry->d_name);
            strcpy(fileList[fileCounter], entry->d_name);
            fileCounter++;
          }
          currentDirSize = fileCounter;
          printf("%i currentdirsize\n", currentDirSize);
          oldScrollOffset = scrollOfset;
          oldSelectOffset = selectOffset;
          scrollOfset = 0;
          selectOffset = 0;
        }
      }

      if (IsKeyPressed(KEY_H))
      {
        closedir(currentDir);
        currentDir = opendir("..");
        chdir("..");
        fileSelection = 0;
        for (int i = 0; i<500; i++)
        {
          *fileList[i] = 0;
        }
        struct dirent *entry;
        int fileCounter = 0;
        while ((entry = readdir(currentDir)) != NULL)
        {
          printf("%s\n", entry->d_name);
          strcpy(fileList[fileCounter], entry->d_name);
          fileCounter++;
        }
        currentDirSize = fileCounter;
        printf("%i currentdirsize\n", currentDirSize);
        scrollOfset = 0;
      }
    }

    // Mouse actions

    Vector2 mposWhenPressedR;
    Vector2 mposWhenPressedL;
    Vector2 wposWhenPressed;
    Vector2 wsizeWhenPressed;
    int butXStart = (screenWidth - (bar_t.width+10));
    if (GetMouseX() > butXStart + 12 && GetMouseX() < butXStart + 12 + cog_t.width && GetMouseY() > 17 && GetMouseY() < 17 + cog_t.height)
    {
      oncog = 1;
    }
    else if (GetMouseX() > butXStart + cog_t.width + 19 && GetMouseX() < butXStart + 19 + cog_t.width + minus_t.width && GetMouseY() > 18 && GetMouseY() < 18 + minus_t.height)
    {
      onminus = 1;
    }
    if (GetMouseX() > butXStart + cog_t.width + minus_t.width + 26 && GetMouseX() < butXStart + 26 + minus_t.width + cross_t.width + cog_t.width && GetMouseY() > 17 && GetMouseY() < 17 + cross_t.height)
    {
      oncross = 1;
    }

    // update shader
    shader = initializeAndUpdateShader(time, oncog, onminus, oncross, screenWidth);

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
      mposWhenPressedR = getFullMousePos();
      wposWhenPressed = GetWindowPosition();
    }
    
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {  
      SetWindowPosition(wposWhenPressed.x + (getFullMousePos().x - mposWhenPressedR.x), wposWhenPressed.y + (getFullMousePos().y - mposWhenPressedR.y));
    }
    
    if (GetMouseX() > screenWidth - resizeMargin && GetMouseY() > screenHeight - resizeMargin)
    {
      SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
    }
    else
    {
      if (!isResizing) SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }


    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {  
      if (GetMouseX() > screenWidth - resizeMargin && GetMouseY() > screenHeight - resizeMargin)
      {
        isResizing = true;
        mposWhenPressedL = getFullMousePos();
        wsizeWhenPressed.x = screenWidth; wsizeWhenPressed.y = screenHeight;
      }
      else if (oncog == 1)
      {
        // TODO
        printf("COG");
        if (cogging)
        {
          shouldBlur = 0;
          cogging = false;
        } 
        else 
        {
          shouldBlur = 1;
          cogging = true; 
        }
      }
      else if (onminus == 1)
      {
        minimizeAtEndOfLoop = 1;
        minLerpVal = 0;
        minOpa = 1;
      }
      else if (oncross == 1)
      {
        uph = -screenHeight;
        downh = screenHeight;
        closeAtEndOfLoop = 1;
      }
    }
   
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
      if (isResizing) 
      {
        screenWidth = (wsizeWhenPressed.x + ((getFullMousePos().x - mposWhenPressedL.x)) - ((int)(wsizeWhenPressed.x + (getFullMousePos().x - mposWhenPressedL.x)) % glphWidth));
        screenHeight = 10 + (wsizeWhenPressed.y + ((getFullMousePos().y - mposWhenPressedL.y)) - ((int)(wsizeWhenPressed.y + (getFullMousePos().y - mposWhenPressedL.y)) % lineHeight));
      }
    }
    
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
      isResizing = false;
      SetMouseCursor(MOUSE_CURSOR_DEFAULT);
      UnloadRenderTexture(tex);
      tex = LoadRenderTexture(screenWidth, screenHeight);
      UnloadRenderTexture(tex2);
      tex2 = LoadRenderTexture(screenWidth, screenHeight);
    }
    
    // textCursor handling    

    int minWidth = (20.9 * glphWidth);
    int minHeight = 10 + (10 * lineHeight);
    if (screenWidth < minWidth) screenWidth = minWidth;
    if (screenHeight < minHeight) screenHeight = minHeight;


    
    
    
    shaderBlur = initializeAndUpdateShaderBlur(shouldBlur, screenWidth, screenHeight);
    
    // if (scrollPos < 0) scrollPos = 0;
    
    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    {
      if (!isResizing && cogging){
      BeginTextureMode(tex);
      }
  
      DrawRectangleGradientEx((Rectangle){0, 0, screenWidth, screenHeight}, grad2, grad1,grad2,grad3);
      int spacing = 0;
      fileCounter = 0;
      Color peekCol = {0,0,0,30};
      // drawing files
      while (fileCounter < currentDirSize)
      {
        printf("%s\n", fileList[fileCounter]);
        int r = stat(fileList[fileCounter], &fs);
        if ( r==-1) {puts("error reading filename with stat"); exit(1);}
        char str[1000];
        if (S_ISREG(fs.st_mode))
        { 
          if (fileCounter == fileSelection)
          {
            FILE * fp = fopen(fileList[fileCounter], "r");
            int headSpacing = 10;
            char fileHead[1024];
            while (fgets(fileHead, sizeof(fileHead),fp) != NULL && headSpacing / lineHeight < 200)
            {
              peekFileLength = headSpacing / lineHeight;
              if (peekFileLength == 199) strcpy(fileHead, "\t\t\t\t\tTHE FILE CONTINUES FURTHER...");
              if (peekMode) peekCol.a = 200;
              DrawTextEx(myFont, fileHead, (Vector2){10,headSpacing - (peekOffset *lineHeight)},textSize,0,peekCol);
              headSpacing += lineHeight;
            }
            fclose(fp);
          }

          if (!peekMode)
          {
            sprintf(str, "%s", fileList[fileCounter]);
            char toTokenize[200];
            strcpy(toTokenize, str);
            char * match;
            char oldmatch[100];
            match = strtok(toTokenize, ".");
            while (match)
            {
              strcpy(oldmatch, match);
              match = strtok(NULL, ".");
            }
            int seedVal = 0;
            for (int i = 0; i<strlen(oldmatch); i++) seedVal += (int)oldmatch[i]*(int)oldmatch[i];
            SetRandomSeed(seedVal);
            Color fileCol = {GetRandomValue(0,255),GetRandomValue(0,255),GetRandomValue(0,255),100};
            DrawRectangle(200, 100+spacing + (scrollOfset * lineHeight), strlen(str) * glphWidth, lineHeight, fileCol);
            DrawTextEx(myFont, str, (Vector2){200, 100 + spacing + 1 + (scrollOfset*lineHeight)}, textSize, 0, BLACK);
            DrawTextEx(myFont, str, (Vector2){200, 100 + spacing + (scrollOfset * lineHeight)}, textSize, 0, WHITE);
          }
        }
        else if (S_ISDIR(fs.st_mode))
        { 
          if (fileCounter == fileSelection)
          {
            DIR * dp = opendir(fileList[fileCounter]);
            int headSpacing = 10;
            struct dirent *dirEntry;
            while ((dirEntry = readdir(dp)) != NULL)
            {
              if (peekMode) peekCol.a = 200;
              DrawTextEx(myFont, dirEntry->d_name, (Vector2){10,headSpacing},textSize,0,peekCol);
              headSpacing += lineHeight;
            }
            closedir(dp);
          }
          if (!peekMode)
          {
            sprintf(str, "\\%s", fileList[fileCounter]);
            DrawTextEx(myFont, str, (Vector2){200, 100 + spacing + (scrollOfset * lineHeight)}, textSize, 0, BLACK);
          }
        }
        if (fileCounter == fileSelection && !peekMode)
        {
          // DrawRectangleRounded((Rectangle){200-glphWidth, 100+spacing, glphWidth*(2+strlen(str)), lineHeight},20,5,(Color){100,100,250,30}); 
          DrawRectangleRounded((Rectangle){200-glphWidth, 100+spacing + (scrollOfset * lineHeight), glphWidth*(2+strlen(str)), lineHeight},20,5,(Color){100,100,250,255}); 
          DrawTextEx(myFont, str, (Vector2){200, 100 + spacing + (lineHeight * scrollOfset)}, textSize, 0, WHITE);
          // DrawCircle(199+((1+strlen(str))*glphWidth), 99+spacing+(lineHeight / 2.0), 4, WHITE);
          // DrawCircle(201+((1+strlen(str))*glphWidth), 101+spacing+(lineHeight / 2.0), 4, DARKBLUE);
          // DrawCircle(200+((1+strlen(str))*glphWidth), 100+spacing+(lineHeight / 2.0), 4, BLUE);
          // DrawCircle(199-glphWidth, 99+spacing+(lineHeight / 2.0), 4, WHITE);
          // DrawCircle(201-glphWidth, 101+spacing+(lineHeight / 2.0), 4, DARKBLUE);
          // DrawCircle(200-glphWidth, 100+spacing+(lineHeight / 2.0), 4, BLUE);
        }
        spacing += lineHeight;
        fileCounter++;
      }
      if(!peekMode)
      {
        char cwdBuf[300];
        getcwd(cwdBuf, 300);
        DrawRectangleRounded((Rectangle){10, screenHeight-(lineHeight+10), 20+ strlen(cwdBuf)*glphWidth,lineHeight},20,5,BLUE);
        DrawTextEx(myFont, cwdBuf, (Vector2){20, screenHeight - (lineHeight+10)}, textSize, 0, BLACK);
      }

      // file drawing
      
      if (!isResizing && cogging){
        EndTextureMode();
        while(!IsTextureReady(tex.texture));
        BeginTextureMode(tex2);
        BeginShaderMode(shaderBlur);
        DrawTexturePro(tex.texture, (Rectangle){0,0,-screenWidth, screenHeight}, (Rectangle){0,0,screenWidth, screenHeight}, (Vector2){screenWidth,screenHeight}, 180, WHITE);
        EndShaderMode();
        EndTextureMode();
        BeginShaderMode(shaderBlur);
        DrawTexturePro(tex2.texture, (Rectangle){0,0,-screenWidth, screenHeight}, (Rectangle){0,0,screenWidth, screenHeight}, (Vector2){screenWidth,screenHeight}, 180, WHITE);
        EndShaderMode();
        int hw = screenWidth / 2.0;
        int hh = screenHeight / 2.0;
        DrawRectangleRec((Rectangle){0, 0, screenWidth, screenHeight}, (Color){150,150,255,40});
        char *settingText = "Press ^c to generate a config.txt"
                            "\n^s - save"
                            "\nhjkl - move around."
                            "\ni - insert mode."
                            "\nesc - back to normal mode."
                            "\no - new line";
        DrawTextEx(myFont, settingText, (Vector2){hw-(glphWidth * 16)+1, hh-(lineHeight*4)+1},textSize,0,(Color){200,200,200,150});
        DrawTextEx(myFont, settingText, (Vector2){hw-(glphWidth * 16)-1, hh-(lineHeight*4)-1},textSize,0,(Color){0,0,0,250});
        DrawTextEx(myFont, settingText, (Vector2){hw-(glphWidth * 16), hh-(lineHeight*4)},textSize,0,(Color){120,80,80,250});
      }
      // DrawTextureEx(tex.texture, (Vector2){0,0}, 20,1, WHITE);



      DrawTexture(bar_t, screenWidth - (bar_t.width + 10), 10,WHITE);

      BeginShaderMode(shader);
      {
        DrawTexture(cog_t, (screenWidth - (bar_t.width+10))+12, 17,WHITE);
        DrawTexture(minus_t, (screenWidth - (bar_t.width+10))+(cog_t.width + 19), 18,WHITE);
        DrawTexture(cross_t, (screenWidth - (bar_t.width+10))+(cog_t.width + minus_t.width + 26), 17,WHITE);
      }
      EndShaderMode();


      DrawLineEx((Vector2){screenWidth-10,screenHeight},(Vector2){screenWidth,screenHeight-10},3, edgeDark);
      DrawLineEx((Vector2){1,1},(Vector2){1,screenHeight},3, edgeLight);
      DrawLineEx((Vector2){1,1},(Vector2){screenWidth,1},3, edgeLight);
      DrawLineEx((Vector2){screenWidth,1},(Vector2){screenWidth,screenHeight},3, edgeDark);
      DrawLineEx((Vector2){1,screenHeight},(Vector2){screenWidth,screenHeight},3, edgeDark);
      
    }

    if (minimizeAtEndOfLoop > 0) 
    {
      if (minimizeAtEndOfLoop > 20){
        minimizeAtEndOfLoop = 0;
        SetWindowState(FLAG_WINDOW_MINIMIZED); 
      }
      else
      {
        minimizeAtEndOfLoop++; 
        
        minLerpVal += 1/25.0;
        minOpa = nLerp(minOpa, 0, minLerpVal);
        SetWindowOpacity(minOpa);
      }
    }
    else SetWindowOpacity(1);
    if (closeAtEndOfLoop > 0) 
    {
      if (closeAtEndOfLoop > 20) break;
      else
      {
        closeAtEndOfLoop++; 
        
        closeLerpVal += 1/25.0;
        uph = nLerp(uph, -(screenHeight / 2.0), closeLerpVal);
        downh = nLerp(downh, screenHeight / 2.0, closeLerpVal);
        DrawRectangle(0,uph,screenWidth, screenHeight, BLACK);
        DrawRectangle(0,downh, screenWidth, screenHeight, BLACK);
      }
    }
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  UnloadTexture(tex.texture);
  UnloadRenderTexture(tex);
  UnloadRenderTexture(tex2);
  UnloadTexture(bar_t);
  UnloadTexture(cog_t);
  UnloadTexture(minus_t);
  UnloadTexture(cross_t);
  //--------------------------------------------------------------------------------------

  return 0;
}
// That's it!





