#ifndef MAIN_H
#define MAIN_H
#include "raylib.h"
#include "./text.h"
#include "./interface.h"
#include "./file2.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
