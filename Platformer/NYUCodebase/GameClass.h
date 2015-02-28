#pragma once
#include "Entity.h"
#include <vector>
#include <string>
#define TIMESTEP 0.0166f
#define MAX_STEPS 6
#define GRAVITY -10.0f
#define FRICTION 5.4f
#define MOVE 1.9f
#define JUMP 2.1f

class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:
	SDL_Window* displayWindow;

	TextureData sheet;
	std::vector<Entity> statics, dynamics;

	float lastTickCount, leftover;
	
	void fillEntities();

	void physics();

	void movePlayer();
	StateAndRun updateGame();
	
	void renderGame();
	
};