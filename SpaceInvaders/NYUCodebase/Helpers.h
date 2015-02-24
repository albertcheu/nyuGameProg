#pragma once
#include "theSDLheaders.h"
#include "Entity.h"

enum ObjLoc{
	GAME, START, PAUSE, WIN, LOSE,
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
