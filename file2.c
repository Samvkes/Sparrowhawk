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



char * getCurrentDirName()
{
  char buf[500];
  getcwd(buf, 500);  
  char *match;
  char *oldmatch = malloc(100);
  match = strtok(buf, "\\");
  while (match != NULL)
  {
    strcpy(oldmatch, match); 
    match = strtok(NULL, "\\");
  }
  return oldmatch;
}


void initializeFileBrowser()
{
  // settings

  //--------------------------------------------------------------------------------------
  for (int i = 0; i<500; i++)
  {
    bs.fileList[i] = malloc(sizeof(char) * 500);    
    *bs.fileList[i] = 0;
  }

  for (int i = 0; i<500; i++)
  {
    bs.higherList[i] = malloc(sizeof(char) * 500);    
    *bs.higherList[i] = 0;
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
  bs.peekFileLength = 0;
  bs.currentDirName = getCurrentDirName();
  bs.offsetInHigher = 0;
  bs.bias = 0;
  shakeScreen(0,0,true);
  // strcpy(bs.currentDirName, getCurrentDirName());
  printf("\n\nDIT: %s\n\n",bs.currentDirName);

  DIR *higher = opendir("..");
  fileCounter = 0;
  while ((entry = readdir(higher)) != NULL)
  {
    if (!strcmp(entry->d_name, bs.currentDirName)) bs.offsetInHigher = fileCounter-1;
    strcpy(bs.higherList[fileCounter], entry->d_name);
    fileCounter++;
  }
  closedir(higher);
}


void handleFileBrowserInputs()
{
	struct stat fs;
  // file actions
  if (IsKeyPressed(KEY_SPACE))
  {
    shakeScreen(1,1,true);
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
    else if (!bs.peekMode && pressedCounter > speedTrigger)
    {
      if (bs.bias > -30) bs.bias--;
      delay = fmax((int)(3 * ((1/60.0) / GetFrameTime())), 1);
      if (frameCounter % delay == 0) 
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
    else pressedCounter += 1;
  }

  if (IsKeyDown(KEY_J))
  {
    if (bs.peekOffset < bs.peekFileLength - (screenHeight / lineHeight) + 2 && bs.peekMode) bs.peekOffset += 1;  
    else if (!bs.peekMode && pressedCounter > speedTrigger)
    {
      if (bs.bias < 30) bs.bias++;
      delay = fmax((int)(3 * ((1/60.0) / GetFrameTime())), 1);
      if (frameCounter % delay == 0) 
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
    else pressedCounter += 1;
  }

  if (IsKeyReleased(KEY_J) || IsKeyReleased(KEY_K)) 
  { 
    pressedCounter = 0; 
  } 

  if (!IsKeyDown(KEY_J) && !IsKeyDown(KEY_K))
  {
    if (fabsf(bs.bias) < 3) bs.bias = 0;
    else if (bs.bias < 0) bs.bias += 3;
    else bs.bias -= 3;
  } 
  
  
  if (IsKeyPressed(KEY_L))
  {
    shakeScreen(5,0, true);
    int r = stat(bs.fileList[bs.fileSelection], &fs);
    if ( r==-1) {puts("error reading filename with stat"); exit(1);}
    if (S_ISDIR(fs.st_mode))
    {
      closedir(bs.currentDir);
      bs.currentDir = opendir(bs.fileList[bs.fileSelection]);
      chdir(bs.fileList[bs.fileSelection]);
      bs.fileSelection = 0;
      free(bs.currentDirName);
      bs.currentDirName = getCurrentDirName();
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

      for (int i = 0; i<500; i++)
      {
  	    *bs.higherList[i] = 0;
      }
      DIR *higher = opendir("..");
      fileCounter = 0;

      printf("%s\n", bs.currentDirName);
      while ((entry = readdir(higher)) != NULL)
      {
        if (!strcmp(entry->d_name, bs.currentDirName)) 
        {
          bs.offsetInHigher = fileCounter-1;
          printf("%s %i\n", entry->d_name, bs.offsetInHigher);
        }
        strcpy(bs.higherList[fileCounter], entry->d_name);
        fileCounter++;
      }
      closedir(higher);
    } 
		else 
		{
			bs.selectedFile = bs.fileList[bs.fileSelection];
		}
  }


  if (IsKeyPressed(KEY_H))
  {
    shakeScreen(5,0,true);
    closedir(bs.currentDir);
    bs.currentDir = opendir("..");
    chdir("..");
    free(bs.currentDirName);
    char *pastName = getCurrentDirName();
    bs.currentDirName = pastName;

    bs.fileSelection = bs.offsetInHigher+1;
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
    bs.scrollOfset = -bs.offsetInHigher-1;

    for (int i = 0; i<500; i++)
    {
	    *bs.higherList[i] = 0;
    }

    chdir("..");
    char *newName = getCurrentDirName();
    chdir(pastName);

    DIR *higher = opendir("..");
    if (strcmp(newName,pastName))
    {
      fileCounter = 0;
      while ((entry = readdir(higher)) != NULL)
      {
        if (!strcmp(entry->d_name, bs.currentDirName)) bs.offsetInHigher = fileCounter-1;
        strcpy(bs.higherList[fileCounter], entry->d_name);
        fileCounter++;
      }
    }
    closedir(higher);
    free(newName);
    printf("%i\n\n", bs.offsetInHigher);
  
  }

}

void drawFileBrowser()
{
	struct stat fs;
  Color peekCol = {0,0,0,20};
	int fileCounter = 0;
	int spacing = 0 - (bs.bias*0.8);
  int curCropLength = 35;

  // drawing files
  while (fileCounter < bs.currentDirSize)
  {
    int r = stat(bs.fileList[fileCounter], &fs);
    if ( r==-1) {puts("error reading filename with stat"); exit(1);}
    int filesInDir = 0;
    char str[1000];
    char selStr[1000];
    char infoStr[100];
    if (S_ISREG(fs.st_mode))
    { 
      if (fileCounter == bs.fileSelection)
      {
        FILE * fp = fopen(bs.fileList[fileCounter], "r");
        int headSpacing = 10;
        char fileHead[100];
        while (fgets(fileHead, sizeof(fileHead),fp) != NULL && headSpacing / lineHeight < 100)
        {
          bs.peekFileLength = headSpacing / lineHeight;
          if (bs.peekFileLength == 99) strcpy(fileHead, "\t\t\t\t\tTHE FILE CONTINUES FURTHER...");
          if (bs.peekMode) peekCol.a = 200;
          DrawTextEx(myFont, fileHead, (Vector2){10,headSpacing - (bs.peekOffset *lineHeight)},textSize,0,peekCol);
          headSpacing += lineHeight;
        }
        fclose(fp);
      }

      if (!bs.peekMode)
      {
        sprintf(selStr, "%s", bs.fileList[fileCounter]);
        if (strlen(bs.fileList[fileCounter]) > curCropLength)
        {
          sprintf(str, "%.*s...", curCropLength, bs.fileList[fileCounter]);
        }
        else sprintf(str, "%s", bs.fileList[fileCounter]);
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
        if (fileCounter != bs.fileSelection)
        {
          DrawRectangle((screenWidth / 2.0) - ((strlen(str)*glphWidth) / 2.0), (screenHeight/2.0) - (lineHeight/2.0)+spacing + (bs.scrollOfset * lineHeight), strlen(str) * glphWidth, lineHeight, fileCol);
          DrawTextEx(myFont, str, (Vector2){(screenWidth / 2.0) - ((strlen(str)*glphWidth) / 2.0), (screenHeight/2.0) - (lineHeight/2.0) + spacing + 1 + (bs.scrollOfset*lineHeight)}, textSize, 0, BLACK);
          DrawTextEx(myFont, str, (Vector2){(screenWidth / 2.0) - ((strlen(str)*glphWidth) / 2.0), (screenHeight/2.0) - (lineHeight/2.0) + spacing + (bs.scrollOfset * lineHeight)}, textSize, 0, WHITE);
          DrawTextEx(myFont, str, (Vector2){(screenWidth / 2.0) - ((strlen(str)*glphWidth) / 2.0), (bs.currentDirSize * lineHeight) + (screenHeight/2.0) - (lineHeight/2.0) + spacing + 60 + (bs.scrollOfset * lineHeight)}, textSize, 0, DARKGRAY);
          DrawTextEx(myFont, str, (Vector2){(screenWidth / 2.0) - ((strlen(str)*glphWidth) / 2.0), (-1 * bs.currentDirSize * lineHeight) + (screenHeight/2.0) - (lineHeight/2.0) + spacing - 60 + (bs.scrollOfset * lineHeight)}, textSize, 0, DARKGRAY);
        }
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
          filesInDir++;
          if (bs.peekMode) peekCol.a = 200;
          DrawTextEx(myFont, dirEntry->d_name, (Vector2){10,headSpacing},textSize,0,peekCol);
          headSpacing += lineHeight;
        }
        closedir(dp);
      }
      if (!bs.peekMode)
      {
        sprintf(selStr, "\\%s", bs.fileList[fileCounter]);
        if (strlen(bs.fileList[fileCounter]) > curCropLength)
        {
          sprintf(str, "\\%.*s...", curCropLength, bs.fileList[fileCounter]);
        }
        else sprintf(str, "\\%s", bs.fileList[fileCounter]);
        
        if (fileCounter != bs.fileSelection)
        {
          DrawTextEx(myFont, str, (Vector2){(screenWidth / 2.0) - ((strlen(str)*glphWidth) / 2.0), (screenHeight/2.0) - (lineHeight/2.0) + spacing + (bs.scrollOfset * lineHeight)}, textSize, 0, BLACK);
          DrawTextEx(myFont, str, (Vector2){(screenWidth / 2.0) - ((strlen(str)*glphWidth) / 2.0), (bs.currentDirSize*lineHeight)+(screenHeight/2.0) - (lineHeight/2.0) + spacing + 60 + (bs.scrollOfset * lineHeight)}, textSize, 0, DARKGRAY);
          DrawTextEx(myFont, str, (Vector2){(screenWidth / 2.0) - ((strlen(str)*glphWidth) / 2.0), (-1 * bs.currentDirSize*lineHeight)+(screenHeight/2.0) - (lineHeight/2.0) + spacing - 60 + (bs.scrollOfset * lineHeight)}, textSize, 0, DARKGRAY);
        }
      }
    }
    if (fileCounter == bs.fileSelection && !bs.peekMode)
    {
      // float hoffset = sin(frameCounter / 50.0) * 10;
      float hoffset = (0.7 - pow(fabs(sin(frameCounter/30.0)), 0.5)) * 15.0;
      spacing += 30;
      Color ourBlue = {100,100,250,255};
      Color shadow = {0,0,0,80 * fmax((hoffset / 10.0),0.3)};
      DrawRectangleGradientH((screenWidth / 2.0) + ((strlen(selStr)*glphWidth) / 2.0) + 8,  hoffset + (screenHeight/2.0) + spacing + (lineHeight * bs.scrollOfset),60,2,ourBlue, WHITE);
      DrawRectangleRounded((Rectangle){(screenWidth / 2.0) - ((strlen(selStr)*glphWidth) / 2.0)-glphWidth, 10 + (screenHeight/2.0) - (lineHeight/2.0)+spacing + (bs.scrollOfset * lineHeight), glphWidth*(2+strlen(selStr)), lineHeight},20,5,shadow); 
      DrawRectangleRounded((Rectangle){(screenWidth / 2.0) - ((strlen(selStr)*glphWidth) / 2.0)-glphWidth, hoffset + (screenHeight/2.0) - (lineHeight/2.0)+spacing + (bs.scrollOfset * lineHeight), glphWidth*(2+strlen(selStr)), lineHeight},20,5,ourBlue); 
      DrawTextEx(myFont, selStr, (Vector2){(screenWidth / 2.0) - ((strlen(selStr)*glphWidth) / 2.0),  hoffset + (screenHeight/2.0) - (lineHeight/2.0)+ spacing + (lineHeight * bs.scrollOfset)}, textSize, 0, WHITE);
      if (filesInDir > 0)
      {
        sprintf(infoStr, "Contains %i file(s)", filesInDir-2); 
      }
      else
      {
        sprintf(infoStr, "Size of  %lu byte(s)", fs.st_size); 
      }
      DrawRectangleRounded((Rectangle){(screenWidth / 2.0) + ((strlen(selStr)*glphWidth) / 2.0) + 65 - 10,  hoffset + (screenHeight/2.0) - (lineHeight/2.0)+ spacing + (lineHeight * bs.scrollOfset), (strlen(infoStr) * glphWidth) + 20, lineHeight},20,5, WHITE);
      DrawTextEx(myFont, infoStr, (Vector2){(screenWidth / 2.0) + ((strlen(selStr)*glphWidth) / 2.0) + 65,  hoffset + (screenHeight/2.0) - (lineHeight/2.0)+ spacing + (lineHeight * bs.scrollOfset)}, textSize, 0, ourBlue);
      spacing += 30;
    }
    spacing += lineHeight;
    fileCounter++;
  }
  if(!bs.peekMode)
  {
    spacing = 0;
    if (strlen(bs.higherList[0]) > 0)
    {
      for (int i = 0; i < 500; i++)
      {
        Color highCol = DARKGRAY;
        float yPos = (screenHeight / 2.0)+spacing - (bs.offsetInHigher*lineHeight) - (lineHeight / 2.0) + 10;
        if (yPos < 200) highCol.a = fmax(0.0,yPos);
        else if (yPos > screenHeight - 200) highCol.a = screenHeight - yPos;
        if (i == bs.offsetInHigher+1) highCol = BLACK;
        if (strlen(bs.higherList[i]) > 15)
        {
          char str[20] = {0};
          sprintf(str, "%.15s...", bs.higherList[i]);
          DrawTextEx(myFont, str, (Vector2){30, yPos}, textSize, 0, highCol);
        }
        else
        {
          DrawTextEx(myFont, bs.higherList[i], (Vector2){30, yPos}, textSize, 0, highCol);
        }
        spacing += lineHeight;
      }
    }
    else
    {
      DrawRectangleGradientH(0,0,50,screenHeight,(Color){0,0,0,100},(Color){0,0,0,0}); 
    }
    char cwdBuf[300];
    getcwd(cwdBuf, 300);
    DrawRectangleRounded((Rectangle){10, screenHeight-(lineHeight+10), 20+ strlen(cwdBuf)*glphWidth,lineHeight},20,5,BLUE);
    DrawTextEx(myFont, cwdBuf, (Vector2){20, screenHeight - (lineHeight+10)}, textSize, 0, BLACK);
    
        
  }

}
