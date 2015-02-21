#pragma once
#include "theSDLheaders.h"
#include "Entity.h"
#include <vector>

#include <sstream>
#include <string>

#define SHIP_WIDTH 0.2f
#define SHIP_SPEED 1.2f
#define BEAM_SPEED 1.7f
#define ENEMIES 15

const float dy[3] = {
	2.5f*84.0f / (WINDOW_HEIGHT / 2.0f),
	1.4f*84.0f / (WINDOW_HEIGHT / 2.0f),
	0.3f*84.0f / (WINDOW_HEIGHT / 2.0f),
};
const float dx[5] = { -0.8f, -0.4f, 0, 0.4f, 0.8f};


class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:	
	enum ObjLoc{
		PB1, PB2, PB3, PB4, PB5, PB6,
		PLAYER, TOP, BOT, LEFT, RIGHT,
		EB1, EB2, EB3, EB4, EB5,
		EB6, EB7, EB8, EB9, EB10,
		ESTART
	};
	
	std::vector<Entity> entities;
	int whichPB, whichEB;
	SDL_Window* displayWindow;
	float lastTickCount, nextShift;

	enum GAMESTATE{ START, GAME, PAUSE};
	int state;

	void fillEntities();
	bool updateGame(float elapsed);
	void renderGame();
	void moveBeams(float elapsed);
	void movePlayer(float elapsed);
	void moveEnemies(float elapsed);

	bool updatePause(float elapsed);
	void renderPause();
};