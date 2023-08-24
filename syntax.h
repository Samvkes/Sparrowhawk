#ifndef SYNTAX_H
#define SYNTAX_H

#include "raylib.h"
#include "./main.h"

// #define STANDARD textCol
#define STANDARD (Color){30,30,30,255}
// #define KEYWORD (Color){130,50,40,255}
// #define KEYWORD (Color){170,40,20,255}
#define KEYWORD (Color){0,0,0,255}
// #define BRACE (Color){40,40,130,255}
#define BRACE_A (Color){40,0,200,255}
#define BRACE_B (Color){0,0,140,255}
// #define STRING (Color){50,100,10,255}
#define STRING (Color){10,100,10,255}
// #define STRING (Color){100,220,130,255}
#define PUNCT (Color){20,20,20,255}
// #define DIGIT (Color){100,50,0,255}
#define DIGIT (Color){100,0,0,255}
#define COMMENT (Color){0,0,0,80}

#define keywordNumber 38

typedef struct syntaxRet{
	Color syntaxColor;
	Font syntaxFont;
} syntaxRet;

syntaxRet syntaxHl(int row, int col, bool newLine, bool newClear); 

#endif