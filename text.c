#include "./text.h"
#include "./interface.h"
#include "./file2.h"
#include "./main.h"
#include "./syntax.h"
#include <stdlib.h>
#include <math.h>
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
  lineCounter = 0;
  bool endsInNl = false;
  // printf("\n\n");
  while(c = text[pos], c != '\0'){
    if (c == '\n') { 
      endsInNl = true;
      lines[lineCounter].start = curStart;
      lines[lineCounter].stop = curStop;
      lineCounter = lineCounter + 1;
      curStart = curStop + 1;
    }
    else endsInNl = false;
    pos++;
    curStop = pos;
  }
  if (endsInNl) {lineCounter--;}
  else {
    lines[lineCounter].stop = pos;
    lines[lineCounter].start = curStart;
  }
  return lines;  
}


void add(char toInsert, int pos)
{
  int textLength = strlen(text);
  if (currentMode != APPEND)
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


void pop(int pos)
{
  int textLength = strlen(text);
  if (currentMode != APPEND)
  {
    if (pos == 0) return;
    for (int i = pos; i<textLength; i++)
    {
      text[i-1] = text[i];
    }
    text[textLength-1] = 0;
  }
  else
  {
    if (pos == 0) currentMode = INSERT;
    for (int i = pos+1; i<textLength; i++)
    {
      text[i-1] = text[i];
    }
    text[textLength-1] = 0;
  }
}


int curToPos(Vector2 textCursor)
{
  // printf("\nSTART\ncursor in curtopos: %i, %i", (int)textCursor.x, (int)textCursor.y);
  int position = lin[(int)textCursor.x].start + (int)textCursor.y;
  //printf("\n%i %i %i\n", position, lin[(int)textCursor.x].start, lin[(int)textCursor.x].stop);
  if (position > lin[(int)textCursor.x].stop)
  {
    currentMode = APPEND;
    position = lin[(int)textCursor.x].stop;
  }
  // printf("new pos: %i\n", position);
  return position;
}


Vector2 posToCur(int position)
{
  Vector2 textCursor = {0, 0};
  int i = 0;
  while (position > lin[i].stop)
  {
    // printf("\nlin[i].start:%i, i: %i\n", lin[i].start, i);
    i++;
  }
  
  textCursor.x = i;
  textCursor.y = position - lin[i].start;
  // printf("new cursor: %f %f \nEND\n", textCursor.x, textCursor.y);
  return textCursor;
}  

int handleText(Vector2 textCursor)
{
  if (textCursor.y == lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start && currentMode == APPEND)
  {
     currentMode = INSERT;
  }
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
  if (key > 32 && key < 126) 
  { 
    if (IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT))
    {
      add(charTable[1][key-39], position);
    }
    else
    {
      // key += 32; 
      add(charTable[0][key-39], position);
      // key -=32;
    }
    position++;
  }
  else if (key == 257) 
  { 
    add('\n', position); 
    lineCounter += 1;
    if (currentMode == APPEND)
    {
      position += 2;
      currentMode = INSERT;
    }
    else position += 1;
  }
  else if (key == 32) { add(' ', position); position++; }
  else if (key == 259) { pop(position); position--;}
  return position;
}


Vector2 checkTextCursor(Vector2 textCursor, bool movedUpOrDown, bool movedLeftOrRight)
{

  if (textCursor.y < 0 && textCursor.x == 0) { textCursor.y = 0; }
  else if (textCursor.y < 0) { textCursor.x--; textCursor.y = (lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start); } 
  if (textCursor.x < 0) textCursor.x = 0;
  if (textCursor.x > lineCounter) textCursor.x = lineCounter;
  
  if (movedUpOrDown)
  {
    if (textCursor.y < cs.past_biggest_y) textCursor.y = cs.past_biggest_y;
  }
  
  if (textCursor.y > ((lin[(int)textCursor.x].stop - lin[(int)textCursor.x].start)))
  { 
    if (!movedUpOrDown && textCursor.x != lineCounter)
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
    cs.past_biggest_y = textCursor.y;
  }

  return textCursor;
}


FILE *initializeText(char *fileName)
{
  puts("hai");
  FILE *fp;
  int offset = 0;
  int c;
  fp = fopen(fileName, "r");
  text = malloc(cs.bufSize);
  puts("2");
  char *newText;
  while (c = fgetc(fp), c != EOF) {
    if (offset == cs.bufSize -1) {
      cs.bufSize = cs.bufSize * 2;
      newText = realloc(text, cs.bufSize); 
      text = newText;
    }
    text[offset++] = c;
  }
  printf("%i\n", offset);
  fclose(fp);
  puts("3");
  if (c == EOF && offset == 0) { free(text); puts("enter a file with content"); exit(1); }
  text[offset] = '\0';
  lin = lines();
  puts("4");
  return fp;
}


void resetCursor()
{
  cs.bufSize = 100;
  cs.textCursor = (Vector2){0,0};
  cs.textPos = (Vector2){30 + (4*glphWidth) + 5, 10 };
  cs.cursorPos = (Vector2){0,0};
  cs.cursorTargetPos = (Vector2){0,0};
  for (int i=0; i<30; i++)
  {
    cs.pastCursorPos[i] = (Vector2){0,0};
  }
  cs.past_biggest_y = 0;
  cs.scrollOfset = 0;
  cs.horOfset = 0;
  cs.lowerHinge = 5;
  cs.upperHinge = 15;
  cs.horHinge = 5;
}


cursorStatus cs;
int toMove = 0;
line *lin;
char *text;
int lineCounter;
char * charTable[] = {"'8888,-./01234567898;8=888abcdefghijklmnopqrstuvwxyz[\\]88`", "\"8888<_>?)!@#$%^&*(8:8+888ABCDEFGHIJKLMNOPQRSTUVWXYZ{|}88~"};
Mode currentMode = NORMAL;
int gutter_frag_size = 20;
Texture2D gutterCollection[3];
Texture2D gutter_mid_t;
Texture2D gutter_top_t;
Texture2D gutter_bot_t;


void initializeTextEditor()
{
  int leftGutterPadding = 10;  
  int gutterWidth = 30;
  int leftMargin = 4 * glphWidth + leftGutterPadding;
  int topMargin = 10;
  int gutterTopMargin = 30;
  float cursorXMicroAdjust = -1;

  Color cursorColor = { 20, 20, 250, 200 };
  Color cursorInnerColor = { 20, 20, 20, 10 };
  Color shadowTextCol = {textCol.r, textCol.g, textCol.b, 60};
  Color lineNumCol = {textCol.r, textCol.g, textCol.b, 180};

  int gutter_frag_size = 20;

  //--------------------------------------------------------------------------------------
  createGutter(gutterCollection, "resources/gutter_long.png", "resources/gutter_dot.png", lineHeight, gutter_frag_size);
  gutter_mid_t = gutterCollection[0];
  gutter_top_t = gutterCollection[1];
  gutter_bot_t = gutterCollection[2];

  cs.textCursor = (Vector2){0,0};
  cs.textPos = (Vector2){gutterWidth + leftMargin, topMargin };
  cs.cursorPos = (Vector2){ 0, 0 };
  cs.cursorTargetPos = (Vector2){ 0, 0 };
  int trailAmount = 15;
  for (int i = 0; i<trailAmount; i++)
  {
    cs.pastCursorPos[i] = (Vector2){0,0};
  }
  cs.scrollOfset = 0;
  cs.horOfset = 0;
  cs.lowerHinge = 5;
  cs.upperHinge = 15;
  cs.horHinge = 5;
  float lerpval = 0;
  bool lerping = false;
}



bool handleTextInput()
{
  static float lerpval = 0;
  static bool lerping = false;
  float lerptime = 0.1;
  int leftGutterPadding = 10;  
  int gutterWidth = 30;
  int leftMargin = 4 * glphWidth + leftGutterPadding;
  int topMargin = 10;
  int gutterTopMargin = 30;
  float cursorXMicroAdjust = -1;

  bool movedUpOrDown = false;
  bool movedLeftOrRight = false;
  if (currentMode == NORMAL)
  {
    if (IsKeyPressed(KEY_Q))
    {
      free(text); 
      free(lin);
      lineCounter = 0;
      return true;
    }
    if (IsKeyPressed(KEY_S) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
    {
      fp = fopen(bs.selectedFile, "w");
      fputs(text, fp);
      fclose(fp);
    }

    if (IsKeyPressed(KEY_J)) 
    { 
      lerpval = 0;
      cs.textCursor.x++;
      movedUpOrDown = true;
    } 
    if (IsKeyPressed(KEY_K)) 
    { 
      lerpval = 0;
      cs.textCursor.x--;
      movedUpOrDown = true;
    } 

    if (IsKeyDown(KEY_J)) 
    { 
      if (pressedCounter > speedTrigger) { 
        if (frameCounter % delay == 0) cs.textCursor.x++; movedUpOrDown = true;
      }
      else { pressedCounter += 1; }
    }
    if (IsKeyDown(KEY_K)) { 
      if (pressedCounter > speedTrigger) { 
        if (frameCounter % delay == 0) cs.textCursor.x--; movedUpOrDown = true;
      }
      else { pressedCounter += 1; }
    }

    if (IsKeyPressed(KEY_L)) 
    { 
      lerpval = 0;
      cs.textCursor.y++;
      movedLeftOrRight = true;
    } 
    if (IsKeyPressed(KEY_H)) 
    { 
      lerpval = 0;
      cs.textCursor.y--;
      movedLeftOrRight = true;
    } 
    if (IsKeyDown(KEY_L)) 
    {
      if (pressedCounter > speedTrigger) { if (frameCounter % delay == 0) cs.textCursor.y++; movedLeftOrRight = true;}
      else { pressedCounter += 1; }
    }
    if (IsKeyDown(KEY_H)) 
    { 
      if (pressedCounter > speedTrigger) { if (frameCounter % delay == 0) cs.textCursor.y--;movedLeftOrRight = true;}
      else { pressedCounter += 1; }
    }

    if (IsKeyReleased(KEY_J) || IsKeyReleased(KEY_K) || IsKeyReleased(KEY_L) || IsKeyReleased(KEY_H)) 
    { 
      pressedCounter = 0; 
    } 
  
    if (IsKeyPressed(KEY_I))
    {
      currentMode = INSERT;
    }
    if (IsKeyPressed(KEY_A))
    {
      currentMode = APPEND;
    }
  }
  else
  {
    if (IsKeyPressed(KEY_ESCAPE))
    {
				currentMode = NORMAL;
    }
    int newPos = handleText(cs.textCursor);
    free(lin);
    lineCounter = 0;
    lin = lines();
    cs.textCursor = posToCur(newPos);
    if (strlen(text) > cs.bufSize * 0.8)
    {
      printf("reallocating! current bufsize: %li\n", cs.bufSize);
      char *newText;
      cs.bufSize = cs.bufSize * 2;
      newText = realloc(text, cs.bufSize); 
      text = newText;
      printf("New bufsize: %li\n", cs.bufSize);
    }
    else if (strlen(text) < cs.bufSize * 0.2)
    {
      printf("reallocating! current bufsize: %li\n", cs.bufSize);
      char *newText;
      cs.bufSize = cs.bufSize * 0.5;
      newText = realloc(text, cs.bufSize); 
      text = newText;
      printf("New bufsize: %li\n", cs.bufSize);
    }
  }

  
  if (cs.cursorPos.x != cs.cursorTargetPos.x || cs.cursorPos.y != cs.cursorTargetPos.y)
  {
    cs.cursorPos.x = nLerp(cs.cursorPos.x, cs.cursorTargetPos.x, lerpval);
    cs.cursorPos.y = nLerp(cs.cursorPos.y, cs.cursorTargetPos.y, lerpval);
    lerpval += lerptime;
  }
  else 
  {
    lerpval = 0;
  }


  
  // textCursor handling    
  cs.textCursor = checkTextCursor(cs.textCursor, movedUpOrDown, movedLeftOrRight);


  int amountOfLines = screenHeight / lineHeight;
  if (amountOfLines < 11)
  {
    cs.lowerHinge = 0;
    cs.upperHinge = amountOfLines; 
  }
  else
  {
    cs.upperHinge = amountOfLines - 5;
    cs.lowerHinge = 5;
  }

  bool notInPlace = true;
  while (notInPlace)
  {
    notInPlace = false;
    cs.cursorTargetPos.x = gutterWidth + leftMargin + ((cs.textCursor.y - cs.horOfset) * glphWidth);
    cs.cursorTargetPos.y = topMargin + ((cs.textCursor.x - cs.scrollOfset) * lineHeight);

    if (cs.cursorTargetPos.y / lineHeight >= cs.upperHinge)
    {
      cs.scrollOfset += 1;
      notInPlace = true;
    }
    if (cs.cursorTargetPos.y / lineHeight <= cs.lowerHinge)
    {
      if (cs.scrollOfset > 0)
      {
        cs.scrollOfset -= 1;
        notInPlace = true;
      }
    }

    if (cs.cursorTargetPos.x / (float)glphWidth >= (screenWidth / (float)glphWidth) - cs.horHinge)
    {
      cs.horOfset += 1;
      notInPlace = true;
    }
    if (cs.cursorTargetPos.x / (float)glphWidth <= ((leftMargin + gutterWidth) / (float)glphWidth) + cs.horHinge)
    {
      if (cs.horOfset > 0)
      {
        cs.horOfset -= 1;
        notInPlace = true;
      }
    }
  }
  return false;
}
  // if (scrollPos < 0) scrollPos = 0;
    
void drawTextEditor()
{
  int leftGutterPadding = 10;  
  int gutterWidth = 30;
  int leftMargin = 4 * glphWidth + leftGutterPadding;
  cs.textPos.x = gutterWidth + leftMargin;
  int topMargin = 10;
  int gutterTopMargin = 30;
  int amountOfLines = screenHeight / lineHeight;
  float cursorXMicroAdjust = -1;
  int trailAmount = 30;
  static int wiggles = 0;

  Color cursorColor = { 20, 20, 250, 200 };
  cursorColor.a = (int)(50 * (1.5 + sin((frameCounter / 15.0))));
  Color cursorInnerColor = { 20, 20, 20, 10 };
  Color shadowTextCol = {textCol.r, textCol.g, textCol.b, 60};
  Color lineNumCol = {textCol.r, textCol.g, textCol.b, 180};
  
  int amountOfGutterPieces = ((screenHeight - (gutter_top_t.height + gutter_bot_t.height + gutterTopMargin + 10)) - wiggles) / gutter_mid_t.height;
  int gutterSpacing = gutterTopMargin + gutter_top_t.height;
  DrawTexture(gutter_top_t, leftMargin, gutterTopMargin, WHITE);
  for (int i = 0; i < amountOfGutterPieces; i++)
  {
    DrawTexture(gutter_mid_t, leftMargin, gutterSpacing, WHITE);
    gutterSpacing += gutter_mid_t.height;
  }
  DrawTexture(gutter_bot_t, leftMargin, gutterSpacing, WHITE);

  // cursor drawing
  if (currentMode == NORMAL)
  {
    Rectangle rec = {cs.cursorPos.x, cs.cursorPos.y, glphWidth, lineHeight};
    DrawRectangleRounded(rec, 0.7, 5, cursorColor);

  }
  else
  {
    if (currentMode == APPEND)
    {
      // DrawRectangle(0, cursorPos.y - 2, screenWidth, lineHeight - 2, cursorInnerColor); 
      // DrawRectangle(cursorPos.x + (int)(glphWidth / 2), 0, glphWidth, screenHeight, cursorInnerColor); 
      DrawRectangle(cs.cursorPos.x+glphWidth+cursorXMicroAdjust, cs.cursorPos.y, glphWidth / 2.5, lineHeight, (Color){cursorColor.r, cursorColor.g, cursorColor.b, cursorColor.a});
    }
    else if (currentMode == INSERT)
    {
      // DrawRectangle(0, cursorPos.y - 2, screenWidth, lineHeight - 2, cursorInnerColor); 
      // DrawRectangle(cursorPos.x - (int)(glphWidth / 2), 0, glphWidth, screenHeight, cursorInnerColor); 
      DrawRectangle(cs.cursorPos.x+cursorXMicroAdjust, cs.cursorPos.y, glphWidth / 2.5, lineHeight, (Color){cursorColor.r, cursorColor.g, cursorColor.b, cursorColor.a});
    }
    else 
    {
      DrawRectangle(0, cs.cursorPos.y - 2, screenWidth, lineHeight - 2, cursorInnerColor); 
      DrawRectangle(cs.cursorPos.x, 0, glphWidth, screenHeight, cursorInnerColor); 
      Rectangle rec = {cs.cursorPos.x, cs.cursorPos.y, glphWidth, lineHeight};
      DrawRectangleRounded(rec, 0.7, 5, (Color){cursorColor.r, cursorColor.g, cursorColor.b, 155});
      for (int i = 0; i<trailAmount; i++)
      {
        Rectangle rec = {cs.pastCursorPos[i].x, cs.pastCursorPos[i].y, glphWidth, lineHeight};
        DrawRectangleRounded(rec, 0.7, 5, (Color){cursorColor.r, cursorColor.g, cursorColor.b, 4});
      }
    }
  }

  int spacing = topMargin;
  for (int i = cs.scrollOfset; i<amountOfLines + cs.scrollOfset; i++)
  {
    bool isThisANewLine = true;
    char regel[200] = {0};
    int regelEnd = lin[i].stop;

    bool doShadowRegel = false;
    char shadowRegel[200] = {0};
    int shadowRegelEnd = lin[i].stop;

    if ((lin[i].start + cs.horOfset + ((screenWidth - (leftMargin + gutterWidth)) / (float)glphWidth)) < regelEnd)
    {
      regelEnd = (lin[i].start + cs.horOfset + ((screenWidth - (leftMargin + gutterWidth)) / glphWidth));
      doShadowRegel = true;
    }


    for (int j=lin[i].start + cs.horOfset; j<regelEnd; j++)
    {
      regel[j-(lin[i].start + cs.horOfset)] = text[j];
    }
  
    if (doShadowRegel)
    {
      DrawRectangleGradientH(screenWidth - 100, spacing, 100, lineHeight, (Color){30, 30, 30, 0}, (Color){30, 30, 30, 60});
      if ((regelEnd + cs.horOfset + ((screenWidth - (leftMargin + gutterWidth)) / glphWidth)) < shadowRegelEnd)
      {
        shadowRegelEnd = (regelEnd + cs.horOfset + ((screenWidth - (leftMargin + gutterWidth)) / glphWidth));
      }
      for (int j=regelEnd; j<shadowRegelEnd; j++)
      {
        shadowRegel[j-(regelEnd)] = text[j];
      }
    }
    cs.textPos.y = spacing;
    if (i <= lineCounter) {
      wiggles = 0;
      if (text[regelEnd] == '\n') DrawTextEx(myFont, "\\n", (Vector2){cs.textPos.x + (strlen(regel) * glphWidth), cs.textPos.y}, textSize, 0, (Color){0,0,0,20}); 
      char str[5];
      sprintf(str, "%4d", i);
      if (i == cs.textCursor.x)
      {
        DrawTextEx(myFont, str, (Vector2){2, spacing}, textSize, 2, (Color){0,0,255,255});
      }
      else 
      {
        DrawTextEx(myFont, str, (Vector2){0, spacing}, textSize, 2, lineNumCol);
      }
      if (i % 2 == 0)
      {
        DrawRectangle(cs.textPos.x - 10, cs.textPos.y, screenWidth - (gutterWidth + leftMargin) + 10, lineHeight, (Color){20,20,20,7});
      }
    }
    else if (i == lineCounter+1)
    {
      wiggles = screenHeight - (cs.textPos.y + (lineHeight/2.0));
      DrawRectangleGradientV(0, cs.textPos.y + (lineHeight/2.0), screenWidth, lineHeight, (Color){20,20,20,50}, (Color){20,20,20,0});
      // DrawRectangleGradientH(0, cs.textPos.y + (lineHeight / 2.0), 30, screenHeight, (Color){20,20,20,50}, (Color){20,20,20,0});
      // char str[5];
      // sprintf(str, "%4s", ":]");
      // DrawTextEx(myFont, str, (Vector2){0, spacing}, textSize, 2, (Color){0,0,0,150});
    }
    // if (doShadowRegel) DrawTextEx(myFont, shadowRegel, (Vector2){cs.textPos.x, cs.textPos.y + (int)((lineHeight + lineHeightPadding) / 2)}, textSize, 2, shadowTextCol);

    bool leadingSpace = true;
    for (int j = 0; j < strlen(regel); j++){
      Color syntaxCol = syntaxHl(i, j, isThisANewLine);
      if (isThisANewLine) isThisANewLine = false;
      if (regel[j] == ' ' && leadingSpace)
      {
        if (j % 2 == 0)
        {
          DrawLineEx((Vector2){(0.4 * glphWidth) + leftMargin + gutterWidth + (j * glphWidth), cs.textPos.y}, (Vector2){(0.4 * glphWidth) + leftMargin + gutterWidth + (j * glphWidth), cs.textPos.y + lineHeight}, 2, (Color){20,20,20,20});
        }
      }
      else
      {
        leadingSpace = false;
        DrawTextCodepoint(myFont, regel[j], (Vector2){cs.textPos.x + (j * glphWidth), cs.textPos.y}, textSize, syntaxCol); 

      }
    }
    spacing += (lineHeight * 1);
  }
  cs.pastCursorPos[frameCounter%trailAmount] = cs.cursorPos;
}
      

    //----------------------------------------------------------------------------------

