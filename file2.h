#ifndef FILE2_H
#define FILE2_H

#include "raylib.h"
#include "./interface.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

typedef struct browserStatus
{
	char *fileList[500];
	char *higherList[500];
	int offsetInHigher;
	DIR *currentDir;
	int currentDirSize;
	char *selectedFile;
	int fileSelection;
  int scrollOfset;
	bool peekMode;
  int peekOffset;
	int peekFileLength;
	char *currentDirName;
	float bias;
} browserStatus;

void initializeFileBrowser();
void handleFileBrowserInputs();
void drawFileBrowser();

extern browserStatus bs;

#endif
