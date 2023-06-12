#ifndef FILE2_H
#define FILE2_H

#include "raylib.h"
#include <dirent.h>

typedef struct browserStatus
{
	char *fileList[500];
	char *higherList[500];
	int offsetInHigher;
	DIR *currentDir;
	int currentDirSize;
	char *selectedFile;
	int fileSelection;
  double scrollOfset;
	bool peekMode;
  int peekOffset;
	int peekFileLength;
	char *currentDirName;
	float bias;
	bool isMoving;
} browserStatus;

void initializeFileBrowser();
void handleFileBrowserInputs();
void drawFileBrowser();

extern browserStatus bs;

#endif
