#ifndef INTERFACE_H
#define INTERFACE_H

#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


extern void createGutter(Texture2D gutterCollection[3], char *gutterpng, char *gutterDotPng, int lineHeight, int gutter_frag_size);
extern void createButtons(Texture2D buttonCollection[4], char *buttonCogBar, char *buttonCogPng, char *buttonMinusPng, char *buttonCrossPng);
extern Shader initializeAndUpdateShader(float time, int oncog, int onminus, int oncross, int screenWidth);
extern Shader initializeAndUpdateShaderBlur(int shouldBlur, int screenWidth, int screenHeight);

#endif