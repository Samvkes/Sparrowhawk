#ifndef TEXT_H
#define TEXT_H
#include "raylib.h"
#include <stdio.h>

typedef struct line {
  int start;
  int stop;
} line;


typedef struct cursorStatus 
{
  Vector2 textCursor;
  Vector2 textPos;
  Vector2 cursorPos;
  Vector2 cursorTargetPos;
  Vector2 pastCursorPos[30];
  int scrollOfset;
  int horOfset;
  int lowerHinge;
  int upperHinge;
  int horHinge;
  int past_biggest_y;
  long bufSize;
} cursorStatus;


typedef enum Mode {NORMAL, INSERT, APPEND} Mode;

extern cursorStatus cs;
extern int toMove;
extern line *lin;
extern char *text;
extern int lineCounter;
extern char * charTable[];
extern Mode currentMode;
extern int gutter_frag_size;
extern Texture2D gutterCollection[3];
extern Texture2D gutter_mid_t;
extern Texture2D gutter_top_t;
extern Texture2D gutter_bot_t;

int calcLinesOnScreen(int scrHeight, int lnHeight);
Vector2 getFullMousePos();
float nLerp(float current, float target, float amount);
float approach(float current, float target, float amount);
line *lines();
void add(char toInsert, int pos);
void pop(int pos);
int curToPos(Vector2 textCursor);
Vector2 posToCur(int position);
int handleText(Vector2 textCursor);
Vector2 checkTextCursor(Vector2 textCursor, bool movedUpOrDown, bool movedLeftOrRight);
FILE *initializeText(char *fileName);
void initializeTextEditor();
bool handleTextInput();
void drawTextEditor();
void resetCursor();

#endif
