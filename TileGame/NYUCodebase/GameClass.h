#pragma once
#include "Entity.h"
#include "Level.h"

#define TIMESTEP 0.0166f
#define MAX_STEPS 6

#define GRAVITY -7.8f
#define FRIC_Y 5.6f
#define JUMP 4.0f

#define FRIC_X 7.7f
#define MOVE 3.8f

#define TILEPIX 16//pixel size of a tile in mfTRO.jpg
#define TILECOUNTX 16//number of tiles in a row in mfTRO.jpg
#define TILECOUNTY 24//number of tiles in a column in mfTRO.jpg

#define PLAYER 0
enum SamusCycles {STANDLEFT, STANDRIGHT, RUNLEFT, RUNRIGHT};

class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:
	SDL_Window* displayWindow;

	Level theLevel;

	TextureData spriteSheet;
	
	std::vector<Dynamic> dynamics;

	Dynamic* player; bool lookLeft;
	std::vector<AnimCycle> cycles;

	float lastTickCount, leftover, frameChange;
	
	void fillEntities();

	void physics();

	void movePlayer(float elapsed);
	StateAndRun updateGame(float elapsed);
	
	void renderGame();
};