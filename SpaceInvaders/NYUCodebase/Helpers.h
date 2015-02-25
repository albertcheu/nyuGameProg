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

enum ObjLoc{
	GAME, START, PAUSE, WIN, LOSE,
	DIGIT_LEFT, DIGIT_MID, DIGIT_RIGHT,
	H1,H2,H3,H4,H5,
	PB1, PB2, PB3, PB4, PB5, PB6,
	PLAYER, TOP, BOT, LEFT, RIGHT,
	EB1, EB2, EB3, EB4, EB5,
	EB6, EB7, EB8, EB9, EB10,
	ESTART
};

typedef struct{
	unsigned newState;
	bool keepRunning;
} StateAndRun;

unsigned getKey();

StateAndRun updateStart();
StateAndRun updatePause();
StateAndRun updateWin();
StateAndRun updateLose();
