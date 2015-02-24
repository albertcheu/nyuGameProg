#pragma once
#include "theSDLheaders.h"
#include "Entity.h"
#include "Helpers.h"

#include <vector>
#include <string>

#define SHIP_WIDTH 0.2f
#define SHIP_SPEED 1.2f
#define BEAM_SPEED 1.7f
#define ENEMY_SPEED 0.08f
#define SPEEDUP 1.08f
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
	TextureData sheet;
	std::vector<Entity> entities; std::vector<Entity> lifeCounters;
	unsigned whichPB, whichEB, health, numAlive;
	SDL_Window* displayWindow;
	float lastTickCount, nextShift, enemySpeed, speedup;
	bool shouldFire;
	int state;

	void fillEntities();void fillLife();void loadScreens();

	void restartGame(float factor);

	void fireBeam(unsigned whichBeam, int dir, unsigned whichShip);

	void movePlayerBeams(float elapsed);	
	void movePlayer(float elapsed);

	//return true if health is 0
	bool moveEnemyBeams(float elapsed);
	//return true if an enemy hits the player or bottom
	bool moveEnemies(float elapsed);
	void enemyFire();

	StateAndRun updateGame(float elapsed);
	void renderGame();
	
};