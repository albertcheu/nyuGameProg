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
#define HEALTH 5

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
	
	TextureData td;
	std::vector<Entity> entities;
	unsigned whichPB, whichEB, health;
	SDL_Window* displayWindow;
	float lastTickCount, nextShift;
	bool shouldFire;

	enum GAMESTATE{ START, GAME, PAUSE};
	int state;

	void restartGame();
	void fillEntities();
	bool updateGame(float elapsed);
	void fireBeam(unsigned whichBeam, int dir, unsigned whichShip);

	void movePlayerBeams(float elapsed);	
	void movePlayer(float elapsed);

	//return true if health is 0
	bool moveEnemyBeams(float elapsed);
	//return true if an enemy hits the player or bottom
	bool moveEnemies(float elapsed);

	void enemyFire();

	void renderGame();

	bool updatePause(float elapsed);
	void renderPause();
};