#include "./main.h"
#include "./text.h"
#include "./interface.h"
#include "./file2.h"
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

Font myFont;
Font boldFont;
Font gutterFont;
int glphWidth;
int lineHeight;
int lineHeightPadding;
bool isResizing;
Color textCol;
int speedTrigger;
int delay;
int textSize;
int screenWidth = 1000;
int screenHeight = 600;
FILE *fp;
int pressedCounter;
int frameCounter;



void shakeScreen(int xStr, int yStr, bool shakeNow)
{
  static int shakeDur = 0;
  static Vector2 oldPos;
  static int xStrength = 0;
  static int yStrength = 0;
  if (shakeNow && shakeDur < 1) 
  {
    shakeDur = 10;
    oldPos = GetWindowPosition();
  }
  else shakeDur--;
  if (xStr > -1) xStrength = xStr;
  if (yStr > -1) yStrength = yStr;
  if (shakeDur > 0)
  {
    SetRandomSeed(frameCounter);
    SetWindowPosition(GetRandomValue(-1 * xStrength, xStrength) + oldPos.x, GetRandomValue(-1 * yStrength, yStrength) + oldPos.y);
  }
  else if (shakeDur == 0) SetWindowPosition(oldPos.x, oldPos.y);
}

int main(int argc, char *argv[])
{
  // Initialization
  //--------------------------------------------------------------------------------------

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Editor - wip");
  SetWindowState(FLAG_WINDOW_UNDECORATED);
  SetWindowPosition(500,200);
  SetTargetFPS(100);               // Set our game to run at 60 frames-per-second
  SetExitKey(KEY_LEFT_ALT);
  puts("\n1\n");
  RenderTexture2D tex;
  RenderTexture2D tex2;
  tex = LoadRenderTexture(screenWidth, screenHeight);
  tex2 = LoadRenderTexture(screenWidth, screenHeight);
  while(!IsRenderTextureReady(tex));
  textSize = 20;
  int hlTextSize = 15;
  puts("\n2\n");
  char startDirBuf[300];
  getcwd(startDirBuf,300);
  // old lineheight: int lineHeight = (int)(myFont.baseSize * 1.5f);
  lineHeightPadding = 2;


  // settings
  char *fontname_std = "resources/JetBrainsMono-Medium.ttf";
  char *fontname_bold = "resources/JetBrainsMono-ExtraBold.ttf";

  char *fontname_gut = "resources/iosevka-slab-light.ttf";
  myFont = LoadFontEx(fontname_std, textSize, 0, 0);
  boldFont = LoadFontEx(fontname_bold, textSize, 0, 0);
  gutterFont = LoadFontEx(fontname_gut, 20, 0, 0);
  SetTextureFilter(myFont.texture, TEXTURE_FILTER_TRILINEAR);
  lineHeight = (int)(myFont.recs[0].height + lineHeightPadding);
  glphWidth = (int)(myFont.recs[0].width);

  puts("\n3\n");
  Color edgeLight = (Color){200,200,200,200};
  Color edgeDark = (Color){0,0,0,100};
  textCol = (Color){20, 20, 50, 255};
  Color grad1 = {130,130,130, 255};
  Color grad2 = {155, 138, 118, 255};
  Color grad3 = {189, 154, 113, 255};
  Color grad4 = {160,169,180, 255};
  Color grad5 = {127, 129, 135, 255};
  Color grad6 = {99, 101, 115, 255};
  // Color grad4 = {217,49,49, 255};
  // Color grad5 = {239, 119, 32, 255};
  // Color grad6 = {209, 154, 10, 255};

  speedTrigger = 20;
  delay = 3;
  int resizeMargin = 20;

  //--------------------------------------------------------------------------------------

  Texture2D buttonCollection[4];
  createButtons(buttonCollection, "resources/Bar.png", "resources/Groupcog.png",  "resources/Vectorminus.png",  "resources/Vectorcross.png");
  Texture2D bar_t = buttonCollection[0];
  Texture2D cog_t = buttonCollection[1];
  Texture2D minus_t = buttonCollection[2];
  Texture2D cross_t = buttonCollection[3];

  puts("\n4\n");
  Shader shader;
  Shader shaderBlur;

  int closeAtEndOfLoop = 0;
  int minimizeAtEndOfLoop = 0;
  isResizing = false;
  pressedCounter = 0;
  frameCounter = 0;
  float lerpval = 0;
  bool lerping = false;
  int newPos = 0;
  int oldPos = 0;
  int shouldBlur = 0;
  bool cogging = 0;
  int uph;
  int downh;
  float closeLerpVal = 0;
  float minLerpVal = 0;
  float minOpa = 1;
  bool currentlyBrowsing = true;
  puts("about to init fb");
  initializeFileBrowser();
  initializeTextEditor();
  puts("filebrowser initialized!");
	
	
  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    shakeScreen(-1,-1,false);
    SetWindowSize(screenWidth, screenHeight);
    int oncog = 0;
    int onminus = 0;
    int oncross = 0;
    frameCounter++;
    float time = GetTime();
    
    if (bs.selectedFile != NULL && currentlyBrowsing)
    {
      resetCursor();
      initializeText(bs.selectedFile);
      puts("gelukt");
      currentlyBrowsing = false;
    }
    if (!cogging)
    {
      if (currentMode == NORMAL){

        if (IsKeyPressed(KEY_EQUAL)) 
        {
          char pastDirBuf[300];
          getcwd(pastDirBuf, 300);
          chdir(startDirBuf);
          textSize += 1;
          hlTextSize += 1;

          UnloadFont(myFont);
          myFont = LoadFontEx(fontname_std, textSize, 0, 0);
          UnloadFont(boldFont);
          boldFont = LoadFontEx(fontname_bold, textSize, 0, 0);
          chdir(pastDirBuf);

          lineHeight = (int)( myFont.recs[0].height + lineHeightPadding);
          glphWidth = (int)(myFont.recs[0].width);
        }

        if (IsKeyPressed(KEY_MINUS))
        {
          char pastDirBuf[300];
          getcwd(pastDirBuf, 300);
          chdir(startDirBuf);
          textSize -= 1;
          hlTextSize -= 1;

          UnloadFont(myFont);
          myFont = LoadFontEx(fontname_std, textSize, 0, 0);
          UnloadFont(boldFont);
          boldFont = LoadFontEx(fontname_bold, textSize, 0, 0);
          chdir(pastDirBuf);

          lineHeight = (int)(myFont.recs[0].height + lineHeightPadding);
          glphWidth = (int)(myFont.recs[0].width);

        }
      }
      if (currentlyBrowsing)
      {
        handleFileBrowserInputs(); 
      }
      else
      {
        if (handleTextInput())
        {
          currentlyBrowsing = true;
          bs.selectedFile = NULL;
        }
      }
    }
    
		// Mouse Actions
		
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
          shakeScreen(10,10,true);
          shouldBlur = 0;
          cogging = false;
        } 
        else 
        {
          shakeScreen(10,10,true);
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

    int minWidth = 30 * glphWidth;
    int minHeight;
    if (closeAtEndOfLoop > 0) minHeight = 1;
    else minHeight = 10 + (10 * lineHeight);
    if (screenWidth < minWidth) screenWidth = minWidth;
    if (screenHeight < minHeight) screenHeight = minHeight;

    shaderBlur = initializeAndUpdateShaderBlur(shouldBlur, screenWidth, screenHeight);
    
    // if (scrollPos < 0) scrollPos = 0;
    
    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    {

      if (!isResizing && cogging) BeginTextureMode(tex);

      {
        if (!currentlyBrowsing)
        {
          DrawRectangleGradientEx((Rectangle){0, 0, screenWidth, screenHeight}, grad2, grad1,grad2,grad3);
        }
        else
        {
          DrawRectangleGradientEx((Rectangle){0, 0, screenWidth, screenHeight}, grad4, grad5,grad6,grad5);
        }
        if (currentlyBrowsing)
        {
          drawFileBrowser();
        }
        else
        {
          drawTextEditor();
        }
            
      }
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
      char fps[10]; 
      sprintf(fps, "%i", GetFPS());
      DrawText(fps, screenWidth - 20, screenHeight-20, 14, BLACK);
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
  UnloadTexture(gutter_mid_t);
  UnloadTexture(gutter_top_t);
  UnloadTexture(gutter_bot_t);
  // free(text);
  // free(lin);
  //--------------------------------------------------------------------------------------

  return 0;
}
// That's it!
