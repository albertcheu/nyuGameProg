#pragma once
#include "Entity.h"
#include <vector>
//#include <string>
#define TIMESTEP 0.0166f
#define MAX_STEPS 6

#define GRAVITY -7.8f
#define FRIC_Y 5.7f
#define JUMP 3.1f

#define FRIC_X 7.0f
#define MOVE 2.8f

#define PLAYER 0
enum SamusCycles {STANDLEFT, STANDRIGHT, RUNLEFT, RUNRIGHT};

class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:
	SDL_Window* displayWindow;
	TextureData sheet;
	std::vector<Entity> statics;
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