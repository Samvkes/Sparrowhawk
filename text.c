#include "raylib.h"
#include "./text.h"
#include "./interface.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

int calcLinesOnScreen(int scrHeight, int lnHeight)
{
  return scrHeight / lnHeight;
}


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


line *lines()
{
  line *lines = calloc(10000, sizeof(line));

  char c;
  int length = strlen(text);
  int pos = 0;
  int curStart = 0;
  int curStop = 0;
  while(c = text[pos], c != '\0'){
    if (c == '\n') { 
      lines[lineCounter].start = curStart;
      lines[lineCounter].stop = curStop;
      lineCounter = lineCounter + 1;
      curStart = curStop + 1;
    }
    pos++;
    curStop = pos;
  }
  return lines;  
}


void add(char toInsert, int pos){
  int textLength = strlen(text);
  if (appendMode == 0)
  {
    for (int i = textLength+1; i>=pos; i--)
    {
      text[i] = text[i-1];
    }
    text[pos] = toInsert;
  }
  else
  {
    for (int i = textLength+1; i>pos; i--)
    {
      text[i] = text[i-1];
    }
    text[pos+1] = toInsert;
    
  }
}


void pop(int pos){
  int textLength = strlen(text);
  if (appendMode == 0)
  {
    for (int i = pos; i<textLength; i++)
    {
      text[i-1] = text[i];
    }
  }
  else
  {
    for (int i = pos+1; i<textLength; i++)
    {
      text[i-1] = text[i];
    }

  }
}


int curToPos(Vector2 textCursor)
{
  //printf("cursor in curtopos: %i, %i", (int)textCursor.x, (int)textCursor.y);
  int position = lin[(int)textCursor.x].start + (int)textCursor.y;
  //printf("\n%i %i %i\n", position, lin[(int)textCursor.x].start, lin[(int)textCursor.x].stop);
  if (position > lin[(int)textCursor.x].stop)
  {
    appendMode = 1;
    position = lin[(int)textCursor.x].stop;
  }
  //printf("new pos: %i\n", position);
  return position;
}


Vector2 posToCur(int position)
{
  Vector2 textCursor = {0, 0};
  int i = 0;
  while (position >= lin[i].start)
  {
    i++;
  }
  i--;
  textCursor.x = i;
  textCursor.y = position - lin[i].start;
  //printf("new cursor: %f %f \n", textCursor.x, textCursor.y);
  return textCursor;
}


int handleText(Vector2 textCursor)
{
  if (textCursor.y == lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start) appendMode = 0;
  int position = curToPos(textCursor);
  static int key = 0;
  static int counter = 0;
  int tmpKey = GetKeyPressed();
  if (IsKeyReleased(key))
  {
    counter = 0;
  }
  if (IsKeyDown(key) && tmpKey == 0)
  {
    if (counter < 40){
      counter++;
      return position;
    }
  }
  else
  {
    key = tmpKey;
    counter = 0;
  }
    
  char *newText;
  if (key == 0) { return position; }
  if (key > 38 && key < 126) 
  { 
    if (key > 64 )
    { 
      if (IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT))
      {
        add((char)key, position);
      }
      else
      {
        key += 32; 
        add((char)key, position);
        key -=32;
      }
    }
    else
    {
      add((char)key, position);
    }
    position++;
  }
  else if (key == 257) 
  { 
    add('\n', position); 
    lineCounter += 1;
    position++;
  }
  else if (key == 32) { add(' ', position); position++; }
  else if (key == 259 && position > 1) { pop(position); position--;}
  return position;
}

Vector2 checkTextCursor(Vector2 textCursor, bool movedUpOrDown, bool movedLeftOrRight, int past_biggest_y)
{

  if (textCursor.y < 0 && textCursor.x == 0) { textCursor.y = 0; }
  else if (textCursor.y < 0) { textCursor.x--; textCursor.y = (lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start); } 
  if (textCursor.x < 0) textCursor.x = 0;
  
  if (movedUpOrDown)
  {
    if (textCursor.y < past_biggest_y) textCursor.y = past_biggest_y;
  }
  
  if (textCursor.y > ((lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start)))
  { 
    if (!movedUpOrDown)
    {
      textCursor.x++;
      textCursor.y = 0; //lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start;
    }
    else
    {
      textCursor.y = ((lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start)); //lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start;
    }
  }

  if (movedLeftOrRight)
  {
    past_biggest_y = textCursor.y;
  }

  return textCursor;
}


FILE *initializeText(int argc, char *argv[])
{
  FILE *fp;
  int bufSize = 100;
  int offset = 0;
  int c;
  if (argc > 1) {
    fp = fopen(argv[1], "r");
    text = malloc(bufSize);
    
    while (c = fgetc(fp), c != EOF) {
      if (offset == bufSize -1) {
        bufSize = bufSize * 2;
        char *newText = realloc(text, bufSize); 
        text = newText;
      }
      text[offset++] = c;
    }
    fclose(fp);
    if (c == EOF && offset == 0) { free(text); puts("enter a file with content"); exit(1); }
    text[offset] = '\0';
  } else { puts("enter a file name please!"); exit(1); } 

  lin = lines();
  return fp;
}

int toMove = 0;
int appendMode = 0;
line *lin;
char *text;
int lineCounter;

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

  FILE *fp = initializeText(argc, argv);

  RenderTexture2D tex;
  RenderTexture2D tex2;
  RenderTexture2D cursorTex;
  RenderTexture2D cursorTex2;
  RenderTexture2D cleanTex;
  Image cleanIm = GenImageColor(100,100,(Color){0,0,0,0});
  tex = LoadRenderTexture(screenWidth, screenHeight);
  tex2 = LoadRenderTexture(screenWidth, screenHeight);
  cursorTex = LoadRenderTexture(100,100);
  cursorTex2 = LoadRenderTexture(100,100);
  cleanTex = LoadRenderTexture(100,100);
  while(!IsRenderTextureReady(tex));
  int textSize = 20;
  int hlTextSize = 15;
  char *fontname_std = "resources/JetBrainsMono-Bold.ttf";
  char *fontname_gut = "resources/iosevka-slab-light.ttf";
  Font myFont = LoadFontEx(fontname_std, textSize, 0, 0);
  Font gutterFont = LoadFontEx(fontname_gut, 20, 0, 0);
  Font highlightFont = LoadFontEx(fontname_std, hlTextSize, 0, 0);
  SetTextureFilter(myFont.texture, TEXTURE_FILTER_TRILINEAR);
  SetTextureFilter(highlightFont.texture, TEXTURE_FILTER_TRILINEAR);
  
  // old lineheight: int lineHeight = (int)(myFont.baseSize * 1.5f);
  int lineHeightPadding = 2;
  int lineHeight = (int)(myFont.recs[0].height + lineHeightPadding);
  int glphWidth = (int)(myFont.recs[0].width);
  int hlGlphWidth = (int)(highlightFont.recs[0].width);


  // settings
  int leftGutterPadding = 5;  
  int gutterWidth = 30;
  int leftMargin = 4 * glphWidth + leftGutterPadding;
  int topMargin = 10;
  int gutterTopMargin = 30;
  float cursorXMicroAdjust = -1;

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
  int lowerHinge = 5;
  int upperHinge = 15;
  int horHinge = 5;
  int gutter_frag_size = 20;

  //--------------------------------------------------------------------------------------
  Texture2D gutterCollection[3];
  createGutter(gutterCollection, "resources/gutter_long.png", "resources/gutter_dot.png", lineHeight, gutter_frag_size);
  Texture2D gutter_mid_t = gutterCollection[0];
  Texture2D gutter_top_t = gutterCollection[1];
  Texture2D gutter_bot_t = gutterCollection[2];

  Texture2D buttonCollection[4];
  createButtons(buttonCollection, "resources/Bar.png", "resources/Groupcog.png",  "resources/Vectorminus.png",  "resources/Vectorcross.png");
  Texture2D bar_t = buttonCollection[0];
  Texture2D cog_t = buttonCollection[1];
  Texture2D minus_t = buttonCollection[2];
  Texture2D cross_t = buttonCollection[3];

  Shader shader;
  Shader shaderBlur;

  Vector2 textCursor = {0,0};
  bool closeAtEndOfLoop = false;
  Vector2 textPos = {gutterWidth + leftMargin, topMargin };
  Vector2 cursorPos = { 0, 0 };
  Vector2 cursorTargetPos = { 0, 0 };
  int trailAmount = 15;
  Vector2 pastCursorPos[trailAmount];
  for (int i = 0; i<trailAmount; i++)
  {
    pastCursorPos[i] = (Vector2){0,0};
  }
  int past_biggest_y = 0;
  int scrollOfset = 0;
  int horOfset = 0;
  bool isResizing = false;
  int pressedCounter = 0;
  int frameCounter = 0;
  float lerpval = 0;
  bool lerping = false;
  int newPos = 0;
  int oldPos = 0;
  int cursorShape = 0;
  bool insertMode = 0;
  int shouldBlur = 0;
  bool cogging = 0;

  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    SetWindowSize(screenWidth, screenHeight);
    cursorColor.a = (int)(50 * (1.5 + sin((frameCounter / 15.0))));
    bool movedUpOrDown = false;
    bool movedLeftOrRight = false;
    int oncog = 0;
    int onminus = 0;
    int oncross = 0;
    frameCounter++;
    float time = GetTime();

    if (!cogging) {
    if (!insertMode){

      if (IsKeyPressed(KEY_EQUAL)) 
      {
        textSize += 1;
        hlTextSize += 1;
        cursorXMicroAdjust -= 0.1;
        UnloadFont(myFont);
        UnloadFont(highlightFont);
        myFont = LoadFontEx(fontname_std, textSize, 0, 0);
        highlightFont = LoadFontEx(fontname_std, hlTextSize, 0, 0);

        lineHeight = (int)( myFont.recs[0].height + lineHeightPadding);
        glphWidth = (int)(myFont.recs[0].width);
        hlGlphWidth = (int)(highlightFont.recs[0].width);
        leftMargin = (4 * glphWidth) + leftGutterPadding;
        textPos.x = gutterWidth + leftMargin;
      }

      if (IsKeyPressed(KEY_MINUS))
      {
        textSize -= 1;
        hlTextSize -= 1;

        if (cursorXMicroAdjust < 0) cursorXMicroAdjust += 0.1;
        UnloadFont(myFont);
        UnloadFont(highlightFont);
        myFont = LoadFontEx(fontname_std, textSize, 0, 0);
        highlightFont = LoadFontEx(fontname_std, hlTextSize, 0, 0);

        lineHeight = (int)(myFont.recs[0].height + lineHeightPadding);
        glphWidth = (int)(myFont.recs[0].width);
        hlGlphWidth = (int)(highlightFont.recs[0].width);
        leftMargin = (4 * glphWidth) + leftGutterPadding;
        textPos.x = gutterWidth + leftMargin;

      }

      if (IsKeyPressed(KEY_S) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
      {
        fp = fopen(argv[1], "w");
        fputs(text, fp);
        fclose(fp);
      }

      if (IsKeyPressed(KEY_J)) 
      { 
        lerpval = 0;
        textCursor.x++;
        movedUpOrDown = true;
      } 
      if (IsKeyPressed(KEY_K)) 
      { 
        lerpval = 0;
        textCursor.x--;
        movedUpOrDown = true;
      } 

      if (IsKeyDown(KEY_J)) 
      { 
        if (pressedCounter > speedTrigger) { 
          if (frameCounter % delay == 0) textCursor.x++; movedUpOrDown = true;
        }
        else { pressedCounter += 1; }
      }
      if (IsKeyDown(KEY_K)) { 
        if (pressedCounter > speedTrigger) { 
          if (frameCounter % delay == 0) textCursor.x--; movedUpOrDown = true;
        }
        else { pressedCounter += 1; }
      }

      if (IsKeyPressed(KEY_L)) 
      { 
        lerpval = 0;
        textCursor.y++;
        movedLeftOrRight = true;
      } 
      if (IsKeyPressed(KEY_H)) 
      { 
        lerpval = 0;
        textCursor.y--;
        movedLeftOrRight = true;
      } 
      if (IsKeyDown(KEY_L)) 
      {
        if (pressedCounter > speedTrigger) { if (frameCounter % delay == 0) textCursor.y++; movedLeftOrRight = true;}
        else { pressedCounter += 1; }
      }
      if (IsKeyDown(KEY_H)) 
      { 
        if (pressedCounter > speedTrigger) { if (frameCounter % delay == 0) textCursor.y--;movedLeftOrRight = true;}
        else { pressedCounter += 1; }
      }
    
      if (IsKeyReleased(KEY_J) || IsKeyReleased(KEY_K) || IsKeyReleased(KEY_L) || IsKeyReleased(KEY_H)) 
      { 
        pressedCounter = 0; 
      } 

      if (IsKeyUp(KEY_J) && IsKeyUp(KEY_K) && IsKeyUp(KEY_L) && IsKeyUp(KEY_H)) 
      { 
        cursorShape = 0;
      } 
      
      if (IsKeyPressed(KEY_I))
      {
        insertMode = 1;
      }
      if (IsKeyPressed(KEY_A))
      {
        insertMode = 1;
        appendMode = 1;
      }
    }
    else
    {
      if (IsKeyPressed(KEY_ESCAPE))
      {
        appendMode = 0;
        insertMode = 0;
      }
      else
      {
        int newPos = handleText(textCursor);
        free(lin);
        lineCounter = 0;
        lin = lines();
        textCursor = posToCur(newPos);
      }
    }
    if (cursorPos.x != cursorTargetPos.x || cursorPos.y != cursorTargetPos.y)
    {
      cursorPos.x = nLerp(cursorPos.x, cursorTargetPos.x, lerpval);
      cursorPos.y = nLerp(cursorPos.y, cursorTargetPos.y, lerpval);
      lerpval += lerptime;
      cursorShape = 1;
    }
    else 
    {
      lerpval = 0;
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
        SetWindowState(FLAG_WINDOW_MINIMIZED); 
      }
      else if (oncross == 1)
      {
        closeAtEndOfLoop = true;
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
    textCursor = checkTextCursor(textCursor, movedUpOrDown, movedLeftOrRight, past_biggest_y);

    int minWidth = (leftMargin + gutterWidth) + (20.9 * glphWidth);
    int minHeight = 10 + (10 * lineHeight);
    if (screenWidth < minWidth) screenWidth = minWidth;
    if (screenHeight < minHeight) screenHeight = minHeight;

    int amountOfLines = screenHeight / lineHeight;
    if (amountOfLines < 11)
    {
      lowerHinge = 0;
      upperHinge = amountOfLines; 
    }
    else
    {
      upperHinge = amountOfLines - 5;
      lowerHinge = 5;
    }

    bool notInPlace = true;
    while (notInPlace)
    {
      notInPlace = false;
      cursorTargetPos.x = gutterWidth + leftMargin + ((textCursor.y - horOfset) * glphWidth);
      cursorTargetPos.y = topMargin + ((textCursor.x - scrollOfset) * lineHeight);

      if (cursorTargetPos.y / lineHeight >= upperHinge)
      {
        scrollOfset += 1;
        notInPlace = true;
      }
      if (cursorTargetPos.y / lineHeight <= lowerHinge)
      {
        if (scrollOfset > 0)
        {
          scrollOfset -= 1;
          notInPlace = true;
        }
      }

      if (cursorTargetPos.x / (float)glphWidth >= (screenWidth / (float)glphWidth) - horHinge)
      {
        horOfset += 1;
        notInPlace = true;
      }
      if (cursorTargetPos.x / (float)glphWidth <= ((leftMargin + gutterWidth) / (float)glphWidth) + horHinge)
      {
        if (horOfset > 0)
        {
          horOfset -= 1;
          notInPlace = true;
        }
      }
    }
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
      int amountOfGutterPieces = (screenHeight - (gutter_top_t.height + gutter_bot_t.height + gutterTopMargin + 10)) / gutter_mid_t.height;
      int gutterSpacing = gutterTopMargin + gutter_top_t.height;
      DrawTexture(gutter_top_t, leftMargin, gutterTopMargin, WHITE);
      for (int i = 0; i < amountOfGutterPieces; i++)
      {
        DrawTexture(gutter_mid_t, leftMargin, gutterSpacing, WHITE);
        gutterSpacing += gutter_mid_t.height;
      }
      DrawTexture(gutter_bot_t, leftMargin, gutterSpacing, WHITE);
      
      // cursor drawing
      if (cursorShape == 0 && insertMode == 0)
      {
        Rectangle rec = {cursorPos.x, cursorPos.y, glphWidth, lineHeight};
        DrawRectangleRounded(rec, 0.7, 5, cursorColor);
        for (int i = 0; i<30; i++)
        {
          Rectangle rec = {pastCursorPos[i].x, pastCursorPos[i].y, glphWidth, lineHeight};
          DrawRectangleRounded(rec, 0.7, 5, (Color){cursorColor.r+60, cursorColor.g, cursorColor.b, 2});
        }

      }
      else
      {
        if (appendMode == 1)
        {
          // DrawRectangle(0, cursorPos.y - 2, screenWidth, lineHeight - 2, cursorInnerColor); 
          // DrawRectangle(cursorPos.x + (int)(glphWidth / 2), 0, glphWidth, screenHeight, cursorInnerColor); 
          DrawRectangle(cursorPos.x+glphWidth+cursorXMicroAdjust, cursorPos.y, glphWidth / 2.5, lineHeight, (Color){cursorColor.r, cursorColor.g, cursorColor.b, cursorColor.a+60});
        }
        else if (insertMode == 1)
        {
          // DrawRectangle(0, cursorPos.y - 2, screenWidth, lineHeight - 2, cursorInnerColor); 
          // DrawRectangle(cursorPos.x - (int)(glphWidth / 2), 0, glphWidth, screenHeight, cursorInnerColor); 
          DrawRectangle(cursorPos.x+cursorXMicroAdjust, cursorPos.y, glphWidth / 2.5, lineHeight, (Color){cursorColor.r, cursorColor.g, cursorColor.b, cursorColor.a+60});
        }
        else 
        {
          DrawRectangle(0, cursorPos.y - 2, screenWidth, lineHeight - 2, cursorInnerColor); 
          DrawRectangle(cursorPos.x, 0, glphWidth, screenHeight, cursorInnerColor); 
          Rectangle rec = {cursorPos.x, cursorPos.y, glphWidth, lineHeight};
          DrawRectangleRounded(rec, 0.7, 5, (Color){cursorColor.r, cursorColor.g, cursorColor.b, 155});
          for (int i = 0; i<trailAmount; i++)
          {
            Rectangle rec = {pastCursorPos[i].x, pastCursorPos[i].y, glphWidth, lineHeight};
            DrawRectangleRounded(rec, 0.7, 5, (Color){cursorColor.r, cursorColor.g, cursorColor.b, 4});
          }
        }
      }
      int spacing = topMargin;
      for (int i = scrollOfset; i<amountOfLines + scrollOfset; i++)
      {
        char regel[200] = {0};
        int regelEnd = lin[i].stop;

        bool doShadowRegel = false;
        char shadowRegel[200] = {0};
        int shadowRegelEnd = lin[i].stop;

        if ((lin[i].start + horOfset + ((screenWidth - (leftMargin + gutterWidth)) / (float)glphWidth)) < regelEnd)
        {
          regelEnd = (lin[i].start + horOfset + ((screenWidth - (leftMargin + gutterWidth)) / glphWidth));
          doShadowRegel = true;
        }


        for (int j=lin[i].start + horOfset; j<regelEnd; j++)
        {
          regel[j-(lin[i].start + horOfset)] = text[j];
        }
        
        if (doShadowRegel)
        {
          DrawRectangleGradientH(screenWidth - 100, spacing, 100, lineHeight, (Color){30, 30, 30, 0}, (Color){30, 30, 30, 60});
          if ((regelEnd + horOfset + ((screenWidth - (leftMargin + gutterWidth)) / hlGlphWidth)) < shadowRegelEnd)
          {
            shadowRegelEnd = (regelEnd + horOfset + ((screenWidth - (leftMargin + gutterWidth)) / hlGlphWidth));
          }
          for (int j=regelEnd; j<shadowRegelEnd; j++)
          {
            shadowRegel[j-(regelEnd)] = text[j];
          }
        }
        textPos.y = spacing;
        char str[5];
        sprintf(str, "%4d", i);
        if (i == textCursor.x)
        {
          DrawTextEx(myFont, str, (Vector2){2, spacing}, textSize, 2, (Color){0,0,255,255});
        }
        else 
        {
          DrawTextEx(myFont, str, (Vector2){0, spacing}, textSize, 2, lineNumCol);
        }
        if (i % 2 == 0)
        {
          DrawRectangle(textPos.x - 10, textPos.y, screenWidth - (gutterWidth + leftMargin) + 10, lineHeight, (Color){20,20,20,7});
        }
        if (doShadowRegel) DrawTextEx(highlightFont, shadowRegel, (Vector2){textPos.x, textPos.y + (int)((lineHeight + lineHeightPadding) / 2)}, hlTextSize, 2, shadowTextCol);
        bool leadingSpace = true;
        for (int j = 0; j < strlen(regel); j++){
          if (regel[j] == ' ' && leadingSpace)
          {
            DrawLineEx((Vector2){(0.4 * glphWidth) + leftMargin + gutterWidth + (j * glphWidth), textPos.y}, (Vector2){(0.4 * glphWidth) + leftMargin + gutterWidth + (j * glphWidth), textPos.y + lineHeight}, 2, (Color){20,20,20,30});
          }
          else
          {
            leadingSpace = false;
            DrawTextCodepoint(myFont, regel[j], (Vector2){textPos.x + (j * glphWidth), textPos.y}, textSize, textCol); 

          }
        }
        spacing += (lineHeight * 1);
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
        DrawTextEx(myFont, settingText, (Vector2){hw-(glphWidth * 16), hh-(lineHeight*4)},textSize,0,textCol);
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

    pastCursorPos[frameCounter%trailAmount] = cursorPos;
    EndDrawing();
    //----------------------------------------------------------------------------------
    if (closeAtEndOfLoop) break;
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  UnloadTexture(tex.texture);
  UnloadRenderTexture(tex);
  UnloadTexture(gutter_mid_t);
  UnloadTexture(gutter_top_t);
  UnloadTexture(gutter_bot_t);
  UnloadTexture(bar_t);
  UnloadTexture(cog_t);
  UnloadTexture(minus_t);
  UnloadTexture(cross_t);
  free(text);
  //--------------------------------------------------------------------------------------

  return 0;
}
// That's it!
