#include "raylib.h"
#include "./text.h"
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


line *lines(char *text, int *lineCounter)
{
  line *lines = calloc(10000, sizeof(line));

  char c;
  int length = strlen(text);
  int pos = 0;
  int curStart = 0;
  int curStop = 0;
  while(c = text[pos], c != '\0'){
    if (c == '\n') { 
      lines[*lineCounter].start = curStart;
      lines[*lineCounter].stop = curStop;
      *lineCounter = *lineCounter + 1;
      curStart = curStop + 1;
    }
    pos++;
    curStop = pos;
  }
  return lines;  
}


void add(char *text, char toInsert, int pos){
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


void pop(char *text, int pos){
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


int handleText(char *text, Vector2 textCursor, int *lineCounter)
{
  if (textCursor.y == lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start) appendMode = 0;
  //printf("cursor in start handletext: %i, %i", (int)textCursor.x, (int)textCursor.y);
  //printf("\n %i %i\n", lin[(int)textCursor.x].start, lin[(int)textCursor.x].stop);
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
    
  char * newText;
  if (key == 0) { return position; }
  if (key > 38 && key < 126) 
  { 
    if (key > 64 )
    { 
      if (IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT))
      {
        add(text, (char)key, position);
      }
      else
      {
        key += 32; 
        add(text, (char)key, position);
        key -=32;
      }
    }
    else
    {
      add(text, (char)key, position);
    }
    position++;
  }
  else if (key == 257) 
  { 
    add(text, '\n', position); 
    *lineCounter += 1;
    position++;
  }
  else if (key == 32) { add(text, ' ', position); position++; }
  else if (key == 259 && position > 1) { pop(text, position); position--;}
  return position;
}


int toMove = 0;
int appendMode = 0;
line *lin;
char *text;

int main(int argc, char *argv[])
{
  // Initialization
  //--------------------------------------------------------------------------------------

  // TODO shader currently only works with screenHeight of 1000
  bool closeAtEndOfLoop = false;
  int screenWidth = 1000;
  int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Editor - wip");
  char *fontname_std = "resources/JetBrainsMono-Bold.ttf";
  char *fontname_gut = "resources/iosevka-slab-light.ttf";
  char *fontname_hl = "resources/JetBrainsMono-Bold.ttf";
  Font myFont = LoadFontEx(fontname_std, 20, 0, 0);
  Font gutterFont = LoadFontEx(fontname_gut, 20, 0, 0);
  Font highlightFont = LoadFontEx(fontname_hl, 15, 0, 0);

  SetTextureFilter(myFont.texture, TEXTURE_FILTER_TRILINEAR);
  SetTextureFilter(highlightFont.texture, TEXTURE_FILTER_TRILINEAR);
  
  SetExitKey(KEY_LEFT_ALT);

  int lineHeight = (int)(myFont.baseSize * 1.5f);
  int glphWidth = (int)(myFont.recs[0].width + 2);
  int hlGlphWidth = (int)(highlightFont.recs[0].width + 2);
  // screenHeight = lineHeight * 30;
  screenHeight = 600;
  SetWindowSize(screenWidth, screenHeight);
  SetWindowState(FLAG_WINDOW_UNDECORATED);
  // SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetWindowPosition(500,200);
  SetTargetFPS(100);               // Set our game to run at 60 frames-per-second
  
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

  int lineCounter = 0;
  lin = lines(text, &lineCounter);
  for (int i = 0; i < lineCounter; i++) 
  {
    //printf("start: %i, stop: %i\n", lin[i].start, lin[i].stop); 
  }
  //printf("\n\nsize of buffer: %i\nSize of offset: %i\nlinecounter= %i\n\n", bufSize, offset, lineCounter);

  // var init
  int leftGutterPadding = 5;  
  int gutterWidth = 30;
  int leftMargin = 4 * glphWidth + leftGutterPadding;
  int topMargin = 10;
  int gutterTopMargin = 30;
  Vector2 cursorPos = { 0, 0 };
  Vector2 cursorTargetPos = { 0, 0 };
  Vector2 textPos = {gutterWidth + leftMargin, topMargin };
  Vector2 textCursor = {0,0};
  int pressedCounter = 0;
  int speedTrigger = 20;
  Color cursorColor = { 20, 20, 20, 100 };
  Color cursorInnerColor = { 20, 20, 20, 10 };
  Color cursorTrailColor = { 20, 20, 20, 50 };
  // Color backCol = {198, 199, 185, 255};
  Color backCol = {97, 113, 103, 255};
  // Color textCol = {76, 71, 64, 255};
  Color textCol = {20, 20, 50, 255};
  Color lineNumCol = {textCol.r, textCol.g, textCol.b, 180};
  int frameCounter = 0;
  float lerpval = 0;
  bool lerping = false;
  int newPos = 0;
  int oldPos = 0;
  int decoButtonWidth = 30;
  int decoButtonHeight = 30;
  int cursorShape = 0;
  bool insertMode = 0;
  int delay = 3;
  float lerptime = 0.1;
  bool isResizing = false;
  int resizeMargin = 20;
  int past_biggest_y = 0;
  int scrollOfset = 0;
  int horOfset = 0;
  int lowerHinge = 5;
  int upperHinge = 15;
  int horHinge = 5;
  int textSize = 20;
  int oncog = 0;
  int onminus = 0;
  int oncross = 0;
  int gutter_frag_size = 20;
  int linesInScreen = screenHeight / lineHeight;
  int gutterLength = screenHeight - 60;
  Image gutter_mid = LoadImage("resources/gutter_long.png");
  ImageCrop(&gutter_mid, (Rectangle){0,20,gutter_mid.width, lineHeight});
  ImageResize(&gutter_mid, gutter_mid.width, gutter_frag_size);
  Texture2D gutter_mid_t = LoadTextureFromImage(gutter_mid);


  Image gutter_top = LoadImage("resources/gutter_dot.png");
  ImageCrop(&gutter_top, (Rectangle){0,0,gutter_mid.width, 20});
  Texture2D gutter_top_t = LoadTextureFromImage(gutter_top);
  Image gutter_bot = LoadImage("resources/gutter_dot.png");
  ImageCrop(&gutter_bot, (Rectangle){0,20,gutter_bot.width, (gutter_bot.height-20)});
  Texture2D gutter_bot_t = LoadTextureFromImage(gutter_bot);
  UnloadImage(gutter_top);
  UnloadImage(gutter_bot);
  Image butts = LoadImage("resources/butts.png");
  Texture2D butts_t = LoadTextureFromImage(butts);

  Image bar = LoadImage("resources/Bar.png");
  Texture2D bar_t = LoadTextureFromImage(bar);
  UnloadImage(bar);
  Image cross = LoadImage("resources/Vectorcross.png");
  Texture2D cross_t = LoadTextureFromImage(cross);
  UnloadImage(cross);
  Image minus = LoadImage("resources/Vectorminus.png");
  Texture2D minus_t = LoadTextureFromImage(minus);
  UnloadImage(minus);
  Image cog = LoadImage("resources/Groupcog.png");
  Texture2D cog_t = LoadTextureFromImage(cog);
  UnloadImage(cog);

  Shader shader = LoadShader(0, "resources/text.fs");
  int frameLoc = GetShaderLocation(shader, "frameLoc");
  SetShaderValue(shader, frameLoc, &frameCounter, SHADER_UNIFORM_FLOAT);
  int cogLoc = GetShaderLocation(shader, "oncog");
  SetShaderValue(shader, cogLoc, &oncog, SHADER_UNIFORM_INT);
  int minusLoc = GetShaderLocation(shader, "onminus");
  SetShaderValue(shader, minusLoc, &onminus, SHADER_UNIFORM_INT);
  int crossLoc = GetShaderLocation(shader, "oncross");
  SetShaderValue(shader, crossLoc, &oncross, SHADER_UNIFORM_INT);
  int widthLoc = GetShaderLocation(shader, "screenWidth");
  SetShaderValue(shader, widthLoc, &screenWidth, SHADER_UNIFORM_INT);

  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    linesInScreen = screenHeight / lineHeight;
    //printf("x: %f, y: %f\n", textCursor.x, textCursor.y);
    frameCounter++;
    float time = GetTime();
    SetShaderValue(shader, frameLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, cogLoc, &oncog, SHADER_UNIFORM_INT);
    SetShaderValue(shader, minusLoc, &onminus, SHADER_UNIFORM_INT);
    SetShaderValue(shader, crossLoc, &oncross, SHADER_UNIFORM_INT);
    SetShaderValue(shader, widthLoc, &screenWidth, SHADER_UNIFORM_INT);

    SetWindowSize(screenWidth, screenHeight);
    bool movedUpOrDown = false;
    bool movedLeftOrRight = false;
    if (!insertMode){

      if (IsKeyPressed(KEY_EQUAL)) 
      {

        textSize += 1;
        UnloadFont(myFont);
        UnloadFont(highlightFont);
        myFont = LoadFontEx(fontname_std, textSize, 0, 0);
        highlightFont = LoadFontEx(fontname_hl, textSize-5, 0, 0);
        lineHeight = (int)(myFont.baseSize * 1.5f);
        hlGlphWidth = (int)(highlightFont.recs[0].width + 2);
        glphWidth = (int)(myFont.recs[0].width + 2);
        leftMargin = (4 * glphWidth) + leftGutterPadding;
        textPos.x = gutterWidth + leftMargin;
      }

      if (IsKeyPressed(KEY_MINUS))
      {
        textSize -= 1;
        UnloadFont(myFont);
        UnloadFont(highlightFont);
        myFont = LoadFontEx(fontname_std, textSize, 0, 0);
        highlightFont = LoadFontEx(fontname_hl, textSize-5, 0, 0);
        lineHeight = (int)(myFont.baseSize * 1.5f);
        hlGlphWidth = (int)(highlightFont.recs[0].width + 2);
        glphWidth = (int)(myFont.recs[0].width + 2);
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
        int newPos = handleText(text, textCursor, &lineCounter);
        free(lin);
        lineCounter = 0;
        lin = lines(text, &lineCounter);
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

    cursorColor.a = (int)(50 * (1.5 + sin((frameCounter / 10.0))));
    Vector2 mposWhenPressedR;
    Vector2 mposWhenPressedL;
    Vector2 wposWhenPressed;
    Vector2 wsizeWhenPressed;
    int butXStart = (screenWidth - (bar_t.width+10));
    oncog = 0;
    onminus = 0;
    oncross = 0;
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
    }
    
    
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
    
    // if (scrollPos < 0) scrollPos = 0;
    
    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    {
      ClearBackground(backCol);
      Color grad1 = {130,130,130, 255};
      Color grad2 = {155, 138, 118, 255};
      Color grad3 = {182, 147, 105, 255};
      Color g1 = {61, 0, 0, 255};
      Color g2 = {0, 0, 94, 255};
      Color g3 = {0, 255, 0, 255};
      Color g4 = {61, 160, 94, 255};

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
      // BeginShaderMode(shader);
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
        //// printf("%s\n", regel);
        //// printf("start en stop: %i, %i\n", lin[i].start, lin[i].stop);
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
        if (doShadowRegel) DrawTextEx(highlightFont, shadowRegel, (Vector2){textPos.x, textPos.y + (int)(lineHeight / 2)}, textSize - 5, 2, (Color){textCol.r, textCol.g, textCol.b, 80});
        DrawTextEx(myFont, regel, textPos, textSize, 2, textCol);
        spacing += (lineHeight * 1);
        // spacing += lineHeight;
      }
      // EndShaderMode();
      // DrawTextEx(myFont, text, textPos, 20, 2, textCol);

      if (cursorShape == 0 && insertMode == 0)
      {
        Rectangle rec = {cursorPos.x-2, cursorPos.y - 2, glphWidth, lineHeight - 2};
        DrawRectangleLinesEx(rec, 2.0f, cursorColor);
      }
      else
      {
        if (appendMode == 1)
        {
          DrawRectangle(0, cursorPos.y - 2, screenWidth, lineHeight - 2, cursorInnerColor); 
          DrawRectangle(cursorPos.x - 2 + (int)(glphWidth / 2), 0, glphWidth, screenHeight, cursorInnerColor); 
          DrawRectangle(cursorPos.x+8, cursorPos.y - 2, glphWidth / 4, lineHeight - 2, cursorColor);
        }
        else 
        {
          DrawRectangle(0, cursorPos.y - 2, screenWidth, lineHeight - 2, cursorInnerColor); 
          DrawRectangle(cursorPos.x-2 - (int)(glphWidth / 2), 0, glphWidth, screenHeight, cursorInnerColor); 
          DrawRectangle(cursorPos.x-3, cursorPos.y - 2, glphWidth / 4, lineHeight - 2, cursorColor);
        }
      }
      DrawTexture(bar_t, screenWidth - (bar_t.width + 10), 10,WHITE);
      BeginShaderMode(shader);
      {
        DrawTexture(cog_t, (screenWidth - (bar_t.width+10))+12, 17,WHITE);
        DrawTexture(minus_t, (screenWidth - (bar_t.width+10))+(cog_t.width + 19), 18,WHITE);
        DrawTexture(cross_t, (screenWidth - (bar_t.width+10))+(cog_t.width + minus_t.width + 26), 17,WHITE);
      }
      EndShaderMode();

      Color edgeLight = (Color){200,200,200,200};
      Color edgeDark = (Color){0,0,0,100};
      // DrawLineEx((Vector2){screenWidth-30,screenHeight},(Vector2){screenWidth,screenHeight-30},1, edgeDark);
      // DrawLineEx((Vector2){screenWidth-30,screenHeight},(Vector2){screenWidth,screenHeight-30},1, edgeLight);
      // DrawLineEx((Vector2){screenWidth-20,screenHeight},(Vector2){screenWidth,screenHeight-20},1, edgeDark);
      DrawLineEx((Vector2){screenWidth-10,screenHeight},(Vector2){screenWidth,screenHeight-10},3, edgeDark);
      DrawLineEx((Vector2){1,1},(Vector2){1,screenHeight},3, edgeLight);
      DrawLineEx((Vector2){1,1},(Vector2){screenWidth,1},3, edgeLight);
      DrawLineEx((Vector2){screenWidth,1},(Vector2){screenWidth,screenHeight},3, edgeDark);
      DrawLineEx((Vector2){1,screenHeight},(Vector2){screenWidth,screenHeight},3, edgeDark);
      
    }
    EndDrawing();
    //----------------------------------------------------------------------------------
    if (closeAtEndOfLoop) break;
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  UnloadImage(gutter_mid);
  UnloadTexture(gutter_mid_t);
  free(text);
  //--------------------------------------------------------------------------------------

  return 0;
}
// That's it!
