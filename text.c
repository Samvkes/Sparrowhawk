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


Vector2 GetFullMousePos()
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
  printf("cursor in curtopos: %i, %i", (int)textCursor.x, (int)textCursor.y);
  int position = lin[(int)textCursor.x].start + (int)textCursor.y;
  printf("\n%i %i %i\n", position, lin[(int)textCursor.x].start, lin[(int)textCursor.x].stop);
  if (position > lin[(int)textCursor.x].stop)
  {
    appendMode = 1;
    position = lin[(int)textCursor.x].stop;
  }
  printf("new pos: %i\n", position);
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
  printf("new cursor: %f %f \n", textCursor.x, textCursor.y);
  return textCursor;
}


int handleText(char *text, Vector2 textCursor, int *lineCounter)
{
  if (textCursor.y == lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start) appendMode = 0;
  printf("cursor in start handletext: %i, %i", (int)textCursor.x, (int)textCursor.y);
  printf("\n %i %i\n", lin[(int)textCursor.x].start, lin[(int)textCursor.x].stop);
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
  int screenWidth = 1000;
  int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "Editor - wip");

  Font myFont = LoadFontEx("resources/JetBrainsMono-Bold.ttf", 20, 0, 0);
  SetTextureFilter(myFont.texture, TEXTURE_FILTER_TRILINEAR);
  
  SetExitKey(KEY_LEFT_ALT);

  int lineHeight = (int)(myFont.baseSize * 1.5f);
  int glphWidth = (int)(myFont.recs[0].width + 2);
  // screenHeight = lineHeight * 30;
  screenHeight = 600;
  SetWindowSize(screenWidth, screenHeight);
  SetWindowState(FLAG_WINDOW_UNDECORATED);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetWindowPosition(0,0);
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
    printf("start: %i, stop: %i\n", lin[i].start, lin[i].stop); 
  }
  printf("\n\nsize of buffer: %i\nSize of offset: %i\nlinecounter= %i\n\n", bufSize, offset, lineCounter);

  // var init
  Vector2 cursorPos = { 0, 0 };
  Vector2 cursorTargetPos = { 0, 0 };
  Vector2 textPos = { 10, 2 };
  Vector2 textCursor = {0,0};
  int pressedCounter = 0;
  int speedTrigger = 20;
  Color cursorColor = { 20, 20, 20, 100 };
  Color cursorInnerColor = { 20, 20, 20, 10 };
  Color cursorTrailColor = { 20, 20, 20, 50 };
  Color backCol = {241, 235, 228, 255};
  Color textCol = {76, 71, 64, 255};
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
  int lowerHinge = 5;
  int upperHinge = 14;

  Shader shader = LoadShader(0, "resources/text.fs");
  int cursorLoc = GetShaderLocation(shader, "cursorLoc");
  SetShaderValue(shader, cursorLoc, &cursorPos, SHADER_UNIFORM_VEC2);
  int frameLoc = GetShaderLocation(shader, "frameLoc");
  SetShaderValue(shader, frameLoc, &frameCounter, SHADER_UNIFORM_FLOAT);
  int isMoving = GetShaderLocation(shader, "isMoving");
  SetShaderValue(shader, isMoving, &cursorShape, SHADER_UNIFORM_INT);

  int shader_screenWidth = GetShaderLocation(shader, "screenWidth");
  int shader_screenHeight = GetShaderLocation(shader, "screenHeight");
  SetShaderValue(shader, shader_screenWidth, &screenWidth, SHADER_UNIFORM_INT);
  SetShaderValue(shader, shader_screenHeight, &screenHeight, SHADER_UNIFORM_INT);
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    printf("x: %f, y: %f\n", textCursor.x, textCursor.y);
    frameCounter++;
    float time = GetTime();
    SetShaderValue(shader, cursorLoc, &cursorPos, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, frameLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, shader_screenWidth, &screenWidth, SHADER_UNIFORM_INT);
    SetShaderValue(shader, shader_screenHeight, &screenHeight, SHADER_UNIFORM_INT);
    SetShaderValue(shader, isMoving, &cursorShape, SHADER_UNIFORM_INT);

    SetWindowSize(screenWidth, screenHeight);
    bool movedUpOrDown = false;
    bool movedLeftOrRight = false;
    if (!insertMode){
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

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
      mposWhenPressedR = GetFullMousePos();
      wposWhenPressed = GetWindowPosition();
    }
    
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {  
      SetWindowPosition(wposWhenPressed.x + (GetFullMousePos().x - mposWhenPressedR.x), wposWhenPressed.y + (GetFullMousePos().y - mposWhenPressedR.y));
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
        mposWhenPressedL = GetFullMousePos();
        wsizeWhenPressed.x = screenWidth; wsizeWhenPressed.y = screenHeight;
      }
    }
    
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
      if (isResizing) 
      {
        screenWidth = wsizeWhenPressed.x + (GetFullMousePos().x - mposWhenPressedL.x);
        screenHeight = wsizeWhenPressed.y + (GetFullMousePos().y - mposWhenPressedL.y);
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

    cursorTargetPos.x = 11 + (textCursor.y * glphWidth);
    cursorTargetPos.y = (textCursor.x - scrollOfset) * lineHeight;
    if (textCursor.x >= upperHinge)
    {
      scrollOfset += 1;
      upperHinge += 1;
      lowerHinge += 1;
    }
    if (textCursor.x <= lowerHinge && textCursor.x > 5)
    {
      scrollOfset -= 1;
      upperHinge -= 1;
      lowerHinge -= 1;
    }
    
    // if (scrollPos < 0) scrollPos = 0;
    
    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    {
      ClearBackground(backCol);

      BeginShaderMode(shader);
      int spacing = 0;
      for (int i = scrollOfset; i<200 + scrollOfset; i++)
      {
        char regel[200] = {0};
        for (int j=lin[i].start; j<(lin[i].stop); j++)
        {
          regel[j-lin[i].start] = text[j];
        }
        // printf("%s\n", regel);
        // printf("start en stop: %i, %i\n", lin[i].start, lin[i].stop);
        textPos.y = spacing;
        DrawTextEx(myFont, regel, textPos, 20, 2, textCol);
        spacing += lineHeight;
      }
      EndShaderMode();
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
      DrawTriangleLines((Vector2){screenWidth - 20, screenHeight}, (Vector2){screenWidth, screenHeight - 20}, (Vector2){screenWidth, screenHeight}, RED);
      
    }
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  free(text);
  //--------------------------------------------------------------------------------------

  return 0;
}
// That's it!
