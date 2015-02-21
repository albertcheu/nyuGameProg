#pragma once
#include "theSDLheaders.h"
#include "Entity.h"
#include <vector>

#include <sstream>
#include <string>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define UNIT_WIDTH 1.33f
#define UNIT_HEIGHT 1.0f
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
	bool update(float elapsed);
	void render();

private:
	

	enum ObjLoc{
		PB1, PB2, PB3, PB4, PB5, PB6,
		PLAYER, TOP, BOT, LEFT, RIGHT,
		EB1, EB2, EB3, EB4, EB5,
		EB6, EB7, EB8, EB9, EB10,
		ESTART
	};

	SDL_Window* displayWindow;
	std::vector<Entity> entities;
	float lastTickCount;
	int whichPB, whichEB;

	void setup();
	void fillEntities();
	void moveBeams(float elapsed);
	void movePlayer(float elapsed);
	void moveEnemies(float elapsed);
};