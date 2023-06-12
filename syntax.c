#include "./syntax.h"
#include "./text.h"
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

char * keywordList[keywordNumber] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern",  "float", "for",
																		 "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch",
																		 "typedef", "union", "unsigned", "void", "volatile", "while"};

Color syntaxHl(int row, int col, bool newLine)
{
	static bool lineComment = false;
	static bool first = true;
	static char* regel;
	static char* woorden;
	static bool stringing = false;
	const char *braces = "(){}[]<>";
	const char *punct = "|!?#@^-+=;':.,";

	if (first)
	{ 
		woorden = calloc(1,sizeof(char));
		regel = calloc(1, sizeof(char));
		first = false;
	}

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
		char *splitString = ".,()[]{} ";
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
				for (int i=0; i<keywordNumber - 1; i++)
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
		return COMMENT;
	}
	
	else if (stringing)
	{
		if (text[lin[row].start + col] == '"') stringing = false;
		return STRING;
	}
	
	if (text[lin[row].start + col] == '"')
	{
		stringing = true;	
		return STRING;
	}

	if (text[lin[row].start + col] == '*' || text[lin[row].start + col] == '&')
	{
		return KEYWORD;
	}


	if (isdigit(text[lin[row].start + col]))
	{
		return DIGIT;
	}
	
	
	if (text[lin[row].start + col] == '/' && text[lin[row].start + col + 1] == '/')
	{
		lineComment = true;
		return COMMENT;
	}
	
	if (woorden[col] == '~')
	{
		return KEYWORD;	
	}
	
	for (int i=0; i<strlen(braces); i++)
	{
		if (woorden[col] == braces[i]) return BRACE;
	}
		
	for (int i=0; i<strlen(punct); i++)
	{
		if (woorden[col] == punct[i]) return PUNCT;
	}
		

	
	return STANDARD;
}