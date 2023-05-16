#ifndef TEXT_H
#define TEXT_H
#include "raylib.h"

typedef struct line {
  int start;
  int stop;
} line;

extern int toMove;
extern line *lin;
extern char *text;
Vector2 GetFullMousePos();
float nLerp(float current, float target, float amount);
float approach(float current, float target, float amount);
line *lines(char *text, int *lineCounter);
int appendMode;


#endif