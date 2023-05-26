#include "raylib.h"
#include "./interface.h"
#include <stdlib.h>
#include "./text.h"
#include "./file2.h"
#include "./main.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

browserStatus bs;
void initializeFileBrowser()
{
  // settings

  //--------------------------------------------------------------------------------------
  for (int i = 0; i<500; i++)
  {
    bs.fileList[i] = malloc(sizeof(char) * 500);    
    *bs.fileList[i] = 0;
  }

  bs.currentDir = opendir(".");
  struct dirent *entry;
  int fileCounter = 0;
  while ((entry = readdir(bs.currentDir)) != NULL)
  {
    strcpy(bs.fileList[fileCounter], entry->d_name);
    fileCounter++;
  }
  bs.currentDirSize = fileCounter;
  bs.fileSelection = 0;
	bs.selectedFile = NULL;
}


void handleFileBrowserInputs()
{
  static int init;
  static int peekFileLength;
  if (init != 999)
  {
    puts("hm");
    peekFileLength = 0;
    init = 999;
  }
	struct stat fs;
  // file actions
  if (IsKeyPressed(KEY_SPACE))
  {
    bs.peekOffset = 0;
    bs.peekMode = !bs.peekMode;
  }

  if (IsKeyPressed(KEY_J))
  {
    if (!bs.peekMode)
    {
      bs.scrollOfset--;
      bs.fileSelection++;
      if (bs.fileSelection > bs.currentDirSize-1) 
      {
        bs.fileSelection = 0;
        bs.scrollOfset = 0;
      }
    }
  }

  if (IsKeyPressed(KEY_K))
  {
    if (!bs.peekMode)
    {
      bs.scrollOfset++;
      bs.fileSelection--;
      if (bs.fileSelection < 0) 
      {
        bs.fileSelection = bs.currentDirSize-1;
        bs.scrollOfset = -(bs.currentDirSize-1);
      }
    }
  }
  
  if (IsKeyDown(KEY_K))
  {
    if (bs.peekOffset > 0 && bs.peekMode) bs.peekOffset -= 1;  
  }

  if (IsKeyDown(KEY_J))
  {
    if (bs.peekOffset < peekFileLength - (screenHeight / lineHeight) + 2 && bs.peekMode) bs.peekOffset += 1;  
  }
  
  
  if (IsKeyPressed(KEY_L))
  {
    int r = stat(bs.fileList[bs.fileSelection], &fs);
    if ( r==-1) {puts("error reading filename with stat"); exit(1);}
    if (S_ISDIR(fs.st_mode))
    {
      closedir(bs.currentDir);
      bs.currentDir = opendir(bs.fileList[bs.fileSelection]);
      chdir(bs.fileList[bs.fileSelection]);
      bs.fileSelection = 0;
      for (int i = 0; i<500; i++)
      {
        *bs.fileList[i] = 0;
      }
      struct dirent *entry;
      int fileCounter = 0;
      while ((entry = readdir(bs.currentDir)) != NULL)
      {
        strcpy(bs.fileList[fileCounter], entry->d_name);
        fileCounter++;
      }
      bs.currentDirSize = fileCounter;
      bs.scrollOfset = 0;
    } 
		else 
		{
			bs.selectedFile = bs.fileList[bs.fileSelection];
		}
  }

  if (IsKeyPressed(KEY_H))
  {
    closedir(bs.currentDir);
    bs.currentDir = opendir("..");
    chdir("..");
    bs.fileSelection = 0;
    for (int i = 0; i<500; i++)
    {
	    *bs.fileList[i] = 0;
    }
    struct dirent *entry;
    int fileCounter = 0;
    while ((entry = readdir(bs.currentDir)) != NULL)
    {
      strcpy(bs.fileList[fileCounter], entry->d_name);
      fileCounter++;
    }
    bs.currentDirSize = fileCounter;
    bs.scrollOfset = 0;
  }

}

void drawFileBrowser()
{
  static int init;
  static int peekFileLength;
  if (init != 999)
  {
    peekFileLength = 0;
    init = 999; 
  }
	struct stat fs;
  Color peekCol = {0,0,0,30};
	int fileCounter = 0;
	int spacing = 0;
  // drawing files
  while (fileCounter < bs.currentDirSize)
  {
    int r = stat(bs.fileList[fileCounter], &fs);
    if ( r==-1) {puts("error reading filename with stat"); exit(1);}
    char str[1000];
    if (S_ISREG(fs.st_mode))
    { 
      if (fileCounter == bs.fileSelection)
      {
        FILE * fp = fopen(bs.fileList[fileCounter], "r");
        int headSpacing = 10;
        char fileHead[1024];
        while (fgets(fileHead, sizeof(fileHead),fp) != NULL && headSpacing / lineHeight < 200)
        {
          peekFileLength = headSpacing / lineHeight;
          if (peekFileLength == 199) strcpy(fileHead, "\t\t\t\t\tTHE FILE CONTINUES FURTHER...");
          if (bs.peekMode) peekCol.a = 200;
          DrawTextEx(myFont, fileHead, (Vector2){10,headSpacing - (bs.peekOffset *lineHeight)},textSize,0,peekCol);
          headSpacing += lineHeight;
        }
        fclose(fp);
      }

      if (!bs.peekMode)
      {
        sprintf(str, "%s", bs.fileList[fileCounter]);
        char toTokenize[200];
        strcpy(toTokenize, str);
        char * match;
        char oldmatch[100];
        match = strtok(toTokenize, ".");
        while (match)
        {
          strcpy(oldmatch, match);
          match = strtok(NULL, "."); 
        }
        int seedVal = 0;
        for (int i = 0; i<strlen(oldmatch); i++) seedVal += (int)oldmatch[i]*(int)oldmatch[i];
        SetRandomSeed(seedVal);
        Color fileCol = {GetRandomValue(0,255),GetRandomValue(0,255),GetRandomValue(0,255),100};
        DrawRectangle(200, 100+spacing + (bs.scrollOfset * lineHeight), strlen(str) * glphWidth, lineHeight, fileCol);
        DrawTextEx(myFont, str, (Vector2){200, 100 + spacing + 1 + (bs.scrollOfset*lineHeight)}, textSize, 0, BLACK);
        DrawTextEx(myFont, str, (Vector2){200, 100 + spacing + (bs.scrollOfset * lineHeight)}, textSize, 0, WHITE);
      }
    }
    else if (S_ISDIR(fs.st_mode))
    { 
      if (fileCounter == bs.fileSelection)
      {
        DIR * dp = opendir(bs.fileList[fileCounter]);
        int headSpacing = 10;
        struct dirent *dirEntry;
        while ((dirEntry = readdir(dp)) != NULL)
        {
          if (bs.peekMode) peekCol.a = 200;
          DrawTextEx(myFont, dirEntry->d_name, (Vector2){10,headSpacing},textSize,0,peekCol);
          headSpacing += lineHeight;
        }
        closedir(dp);
      }
      if (!bs.peekMode)
      {
        sprintf(str, "\\%s", bs.fileList[fileCounter]);
        DrawTextEx(myFont, str, (Vector2){200, 100 + spacing + (bs.scrollOfset * lineHeight)}, textSize, 0, BLACK);
      }
    }
    if (fileCounter == bs.fileSelection && !bs.peekMode)
    {
      // DrawRectangleRounded((Rectangle){200-glphWidth, 100+spacing, glphWidth*(2+strlen(str)), lineHeight},20,5,(Color){100,100,250,30}); 
      DrawRectangleRounded((Rectangle){200-glphWidth, 100+spacing + (bs.scrollOfset * lineHeight), glphWidth*(2+strlen(str)), lineHeight},20,5,(Color){100,100,250,255}); 
      DrawTextEx(myFont, str, (Vector2){200, 100 + spacing + (lineHeight * bs.scrollOfset)}, textSize, 0, WHITE);
      // DrawCircle(199+((1+strlen(str))*glphWidth), 99+spacing+(lineHeight / 2.0), 4, WHITE);
      // DrawCircle(201+((1+strlen(str))*glphWidth), 101+spacing+(lineHeight / 2.0), 4, DARKBLUE);
      // DrawCircle(200+((1+strlen(str))*glphWidth), 100+spacing+(lineHeight / 2.0), 4, BLUE);
      // DrawCircle(199-glphWidth, 99+spacing+(lineHeight / 2.0), 4, WHITE);
      // DrawCircle(201-glphWidth, 101+spacing+(lineHeight / 2.0), 4, DARKBLUE);
      // DrawCircle(200-glphWidth, 100+spacing+(lineHeight / 2.0), 4, BLUE);
    }
    spacing += lineHeight;
    fileCounter++;
  }
  if(!bs.peekMode)
  {
    char cwdBuf[300];
    getcwd(cwdBuf, 300);
    DrawRectangleRounded((Rectangle){10, screenHeight-(lineHeight+10), 20+ strlen(cwdBuf)*glphWidth,lineHeight},20,5,BLUE);
    DrawTextEx(myFont, cwdBuf, (Vector2){20, screenHeight - (lineHeight+10)}, textSize, 0, BLACK);
  }

}
