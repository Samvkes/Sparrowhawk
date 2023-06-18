#include "./syntax.h"
#include "./text.h"
#include "./main.h"
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

char * keywordList[keywordNumber] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern",  "float", "for",
																		 "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch",
																		 "typedef", "union", "unsigned", "void", "volatile", "while", "#include", "#ifndef", "#endif", "#define"};

syntaxRet syntaxHl(int row, int col, bool newLine, bool newClear)
{
	static bool lineComment = false;
	static bool first = true;
	static char* regel;
	static char* woorden;
	static bool stringing = false;
	static int braceList[10000] = {0};
	static int braceCounter = 0;
	// string defined with <> instead of "" 
	const char *openBraces = "({[";
	const char *closedBraces = ")}]";
	const char *splitString = ".,()[]{}; ";
	const char *punct = "|!?#@^-+=;':.,";
	syntaxRet syntax = (syntaxRet){STANDARD, myFont};

	if (first)
	{ 
		woorden = calloc(1,sizeof(char));
		regel = calloc(1, sizeof(char));
		first = false;
	}
	if (newClear) braceCounter = 0;
	if (newLine) 
	{
		stringing = false;
		free(woorden);
		free(regel);
		int spaces = 0;
		lineComment = false;
		regel = calloc(2 + (lin[row].stop - lin[row].start), sizeof(char));
		for (int i = 0; i < (lin[row].stop-lin[row].start)+1; i++)
		{
			regel[i] = text[lin[row].start + i];	
			if (regel[i] == ' ') spaces++;
		}
		woorden = calloc(5 + strlen(regel), sizeof(char));
		for (int i= 0; i<strlen(regel); i++)
		{
			woorden[i] = 	regel[i];
		}
		char *token;
		int counter = 0;
		int cursor = 0;
		int gapCounter = 0;
		int gaps[500] = {0};
		bool wasGap = true;
		bool thisTime = false;
		for (int i=0; i<strlen(regel); i++)
		{
			thisTime = false;
			for (int j=0; j<strlen(splitString); j++)
			{
				if (regel[i] == splitString[j])
				{
					wasGap = true;
					thisTime = true;
					gaps[gapCounter]++;			
					break;
				}	
			}
			if (wasGap && !thisTime)
			{
				wasGap = false;
				gapCounter++;
			}
		}
		// printf("gaps: %i, %i, %i, %i, %i\n", gaps[0], gaps[1], gaps[2], gaps[3], gaps[4]);
		if((token = strtok(regel, splitString)) != NULL)
		{
			do {	
				cursor += gaps[counter];
				if (token[strlen(token)-1] == '\n')
				{
					token[strlen(token)-1] = 0;
				}
				// printf("\nToken '%s'\n", token);
				if (strcmp(token, "") == 0) cursor += 1;
				for (int i=0; i<keywordNumber; i++)
				{
					if (strcmp(token, keywordList[i]) == 0)
					{
						for (int j=0; j<strlen(token); j++)
						{
							woorden[cursor + j] = '~';
						}
					}
				}
				cursor += strlen(token);
				counter++;
			} while ((token = strtok(NULL, splitString)) != NULL);
		}
		// printf("row: %i, %s\n", row, woorden);

	}
	else if (lineComment)
	{
		syntax.syntaxColor = COMMENT;
		syntax.syntaxFont = myFont;
		return syntax;
	}
	
	else if (stringing)
	{
		if (text[lin[row].start + col] == '"') stringing = false;
		syntax.syntaxColor = STRING;
		syntax.syntaxFont = boldFont;
		return syntax;
	}

	
	if (text[lin[row].start + col] == '"')
	{
		stringing = true;	
		syntax.syntaxColor = STRING;
		syntax.syntaxFont = boldFont;
		return syntax;
	}


	if (text[lin[row].start + col] == '*' || text[lin[row].start + col] == '&')
	{
		syntax.syntaxColor = PUNCT;
		syntax.syntaxFont = boldFont;
		return syntax;
	}


	if (isdigit(text[lin[row].start + col]))
	{
		bool onlydigit = true;
		bool breakout = false;
		for (int i=0; i<100; i++)
		{	
			if (isdigit(text[lin[row].start + (col+i)])) continue;

			for (int j=0; j<strlen(splitString); j++)
			{
				if (text[lin[row].start + (col + i)] == splitString[j])
				{ 
					breakout = true;
					break;
				}
			}
			if (breakout) break;
			else onlydigit = false;
		}
		
		breakout = false;
		if (onlydigit)
		{
			for (int i=0; i>-100; i--)
			{
				if (isdigit(text[lin[row].start + (col+i)])) continue;

				for (int j=0; j<strlen(splitString); j++)
				{
					if (text[lin[row].start + (col + i)] == splitString[j]) 
					{
						breakout = true;
						break;
					}
				}
				if (breakout) break;
				else onlydigit = false;
			}

			if (onlydigit)
			{
				syntax.syntaxColor = DIGIT;
				syntax.syntaxFont = boldFont;
				return syntax;
			}
		}
	}
	
	
	if (text[lin[row].start + col] == '/' && text[lin[row].start + col + 1] == '/')
	{
		lineComment = true;
		syntax.syntaxColor = COMMENT;
		syntax.syntaxFont = myFont;
		return syntax;
	}
	
	if (woorden[col] == '~')
	{
		syntax.syntaxColor = KEYWORD;
		syntax.syntaxFont = boldFont;
		return syntax;	
	}
	
	for (int i=0; i<strlen(openBraces); i++)
	{
		if (woorden[col] == openBraces[i]) 
		{
			braceCounter += 1;
			if (braceCounter % 2 == 0) syntax.syntaxColor = BRACE_A;
			else syntax.syntaxColor = BRACE_B;
			syntax.syntaxFont = boldFont;
			return syntax;
		}
	}
	for (int i=0; i<strlen(closedBraces); i++)
	{
		if (woorden[col] == closedBraces[i]) 
		{
			if (braceCounter % 2 == 0) syntax.syntaxColor = BRACE_A;
			else syntax.syntaxColor = BRACE_B;
			braceCounter -= 1;
			syntax.syntaxFont = boldFont;
			return syntax;
		}
	}
		
	for (int i=0; i<strlen(punct); i++)
	{
		if (woorden[col] == punct[i]) 
		{
			syntax.syntaxColor = PUNCT;
			syntax.syntaxFont = boldFont;
			return syntax;
		}
	}
			
	return syntax;
}

