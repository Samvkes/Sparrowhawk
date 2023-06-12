#include "./interface.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


void createGutter(Texture2D gutterCollection[3], char *gutterMidPng, char * gutterDotPng, int lineHeight, int gutter_frag_size)
{
	Image gutter_mid = LoadImage(gutterMidPng);
	ImageCrop(&gutter_mid, (Rectangle){0,20,gutter_mid.width, lineHeight});
	ImageResize(&gutter_mid, gutter_mid.width, gutter_frag_size);
	Texture2D gutter_mid_t = LoadTextureFromImage(gutter_mid);
  UnloadImage(gutter_mid);
  Image gutter_top = LoadImage(gutterDotPng);
  ImageCrop(&gutter_top, (Rectangle){0,0,gutter_top.width, 20});
  Texture2D gutter_top_t = LoadTextureFromImage(gutter_top);
  UnloadImage(gutter_top);
  Image gutter_bot = LoadImage(gutterDotPng);
  ImageCrop(&gutter_bot, (Rectangle){0,20,gutter_bot.width, (gutter_bot.height-20)});
  Texture2D gutter_bot_t = LoadTextureFromImage(gutter_bot);
  UnloadImage(gutter_bot);
	gutterCollection[0] = gutter_mid_t;
	gutterCollection[1] = gutter_top_t;
	gutterCollection[2] = gutter_bot_t;
}


void createButtons(Texture2D buttonCollection[4], char *buttonBarPng, char *buttonCogPng, char *buttonMinusPng, char *buttonCrossPng)
{
  Image bar = LoadImage(buttonBarPng);
	Texture2D bar_t = LoadTextureFromImage(bar);
  UnloadImage(bar);
  Image cog = LoadImage(buttonCogPng);
	Texture2D cog_t = LoadTextureFromImage(cog);
  UnloadImage(cog);
  Image minus = LoadImage(buttonMinusPng);
	Texture2D minus_t = LoadTextureFromImage(minus);
  UnloadImage(minus);
  Image cross = LoadImage(buttonCrossPng);
	Texture2D cross_t = LoadTextureFromImage(cross);
	UnloadImage(cross);
	buttonCollection[0] = bar_t;
	buttonCollection[1] = cog_t;
	buttonCollection[2] = minus_t;
	buttonCollection[3] = cross_t;
}

Shader initializeAndUpdateShader(float time, int oncog, int onminus, int oncross, int screenWidth)
{
	static Shader shader;
	static int timeLoc;
	static int cogLoc;
	static int minusLoc;
	static int crossLoc;
	static int widthLoc;
	static bool isInit = false;
	if (!isInit)
	{
	  shader = LoadShader(0, "resources/buttons.fs");
	  timeLoc = GetShaderLocation(shader, "frameLoc");
	  cogLoc = GetShaderLocation(shader, "oncog");
	  minusLoc = GetShaderLocation(shader, "onminus");
	  crossLoc = GetShaderLocation(shader, "oncross");
	  widthLoc = GetShaderLocation(shader, "screenWidth");
		isInit = true;
	}
  SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
  SetShaderValue(shader, cogLoc, &oncog, SHADER_UNIFORM_INT);
  SetShaderValue(shader, minusLoc, &onminus, SHADER_UNIFORM_INT);
  SetShaderValue(shader, crossLoc, &oncross, SHADER_UNIFORM_INT);
  SetShaderValue(shader, widthLoc, &screenWidth, SHADER_UNIFORM_INT);
	return shader;
}

Shader initializeAndUpdateShaderBlur(int shouldBlur, int screenWidth, int screenHeight)
{
	static Shader shader;
	static int shouldBlurLoc;
	static bool isInit = false;
	static int widthLoc;
	static int heightLoc;
	if (!isInit)
	{
	  shader = LoadShader(0, "resources/blur.fs");
	  shouldBlurLoc = GetShaderLocation(shader, "shouldBlur");
	  widthLoc = GetShaderLocation(shader, "screenWidth");
	  heightLoc = GetShaderLocation(shader, "screenHeight");
		isInit = true;
	}
  SetShaderValue(shader, shouldBlurLoc, &shouldBlur, SHADER_UNIFORM_INT);
  SetShaderValue(shader, widthLoc, &screenWidth, SHADER_UNIFORM_INT);
  SetShaderValue(shader, heightLoc, &screenHeight, SHADER_UNIFORM_INT);
	return shader;
}

Shader initializeAndUpdateShaderBloom(int screenWidth, int screenHeight, Color cursorColor)
{
	static Shader shader;
	static bool isInit = false;
	static int widthLoc;
	static int heightLoc;
	static int ccLoc;
	if (!isInit)
	{
	  shader = LoadShader(0, "resources/bloom.fs");
	  widthLoc = GetShaderLocation(shader, "screenWidth");
	  heightLoc = GetShaderLocation(shader, "screenHeight");
	  ccLoc = GetShaderLocation(shader, "cursorColor");
		isInit = true;
	}
  SetShaderValue(shader, widthLoc, &screenWidth, SHADER_UNIFORM_INT);
  SetShaderValue(shader, heightLoc, &screenHeight, SHADER_UNIFORM_INT);
  SetShaderValue(shader, ccLoc, &cursorColor, SHADER_UNIFORM_VEC4);
	return shader;
}

