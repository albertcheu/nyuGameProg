#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define UNIT_WIDTH 1.33f
#define UNIT_HEIGHT 1.0f

#define KEY_ESCAPE 0
#define KEY_SPACE 1
#define CLOSE_WINDOW 2
#define OTHER 3



typedef struct{
	unsigned newState;
	bool keepRunning;
} StateAndRun;

unsigned getKey();
float lerp(float v0, float v1, float t);
float depenetrate(float c1, float h1, float c2, float h2);