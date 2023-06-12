#ifndef MAIN_H
#define MAIN_H
#include "raylib.h"
#include <stdio.h>

void shakeScreen(int xStr, int yStr, bool shakeNow);

extern Font myFont;
extern Font gutterFont;
extern Font highlightFont;
extern int glphWidth;
extern int lineHeight;
extern int lineHeightPadding;
extern bool isResizing;
extern Color textCol;
extern int speedTrigger;
extern int delay;
extern int textSize;
extern int screenWidth;
extern int screenHeight;
extern FILE *fp;
extern int pressedCounter;
extern int frameCounter;


#endif
