#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define UNIT_WIDTH 1.33f
#define UNIT_HEIGHT 1.0f
#define TILEUNITS 0.07f//size of a tile in OpenGL units

#define OTHER 1
#define CLOSE_WINDOW 0

typedef struct{
	unsigned newState;
	bool keepRunning;
} StateAndRun;

unsigned getKey();
float lerp(float v0, float v1, float t);
float depenetrate(float c1, float h1, float c2, float h2);

bool isSolid(int t, const char* mapName);