#ifndef SYNTAX_H
#define SYNTAX_H

#include "raylib.h"
#include "./main.h"

#define STANDARD textCol
#define KEYWORD (Color){130,50,40,255}
#define BRACE (Color){40,40,130,255}
#define STRING (Color){20,120,20,255}
#define PUNCT (Color){0,0,0,120}
#define DIGIT (Color){100,50,0,255}
#define COMMENT (Color){0,0,0,80}

#define keywordNumber 34

Color syntaxHl(int row, int col, bool newLine); 

#endif