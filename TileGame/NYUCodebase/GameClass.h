#pragma once
#include "Entity.h"
#include <vector>
#include <string>
#define TIMESTEP 0.0166f
#define MAX_STEPS 6

#define GRAVITY -7.8f
#define FRIC_Y 5.7f
#define JUMP 3.1f

#define FRIC_X 7.0f
#define MOVE 2.8f

#define TILEUNITS 0.07f//how big a tile is in OpenGL units
#define TILEPIX 16//how big a tile is in pixels
#define TILECOUNTX 16//number of tiles in a row in mfTRO.jpg
#define TILECOUNTY 24//number of tiles in a column in mfTRO.jpg

#define LEVELWIDTH 10//number of tiles in a row in the level
#define LEVELHEIGHT 8//number of tiles in a column in the level

#define PLAYER 0
enum SamusCycles {STANDLEFT, STANDRIGHT, RUNLEFT, RUNRIGHT};

const float OFFSETX = -TILEUNITS * LEVELWIDTH / 2;
const float OFFSETY = TILEUNITS * LEVELHEIGHT / 2;
void tile2world(float* worldX, float* worldY, int tileCol, int tileRow);
void world2tile(float worldX, float worldY, int* tileCol, int* tileRow);
float tileCollide(float x, float y, float v, float h, bool isY);

class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:
	SDL_Window* displayWindow;
	TextureData tileMap, spriteSheet;
	std::vector<float> tileVerts, tileTexts;
	
	std::vector<Dynamic> dynamics;

	Dynamic* player; bool lookLeft;
	std::vector<AnimCycle> cycles;

	float lastTickCount, leftover, frameChange;
	
	void fillLevel();
	void fillEntities();

	void physics();

	void movePlayer(float elapsed);
	StateAndRun updateGame(float elapsed);
	
	void renderGame();
	void drawLevel();
};