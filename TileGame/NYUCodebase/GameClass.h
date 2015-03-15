#pragma once
#include "Entity.h"
#include <vector>

#define TIMESTEP 0.0166f
#define MAX_STEPS 6

#define GRAVITY -7.8f
#define FRIC_Y 5.6f
#define JUMP 4.0f

#define FRIC_X 7.7f
#define MOVE 3.8f

#define TILEUNITS 0.07f//how big a tile is in OpenGL units
#define TILEPIX 16//how big a tile is in pixels
#define TILECOUNTX 16//number of tiles in a row in mfTRO.jpg
#define TILECOUNTY 24//number of tiles in a column in mfTRO.jpg

#define PLAYER 0
enum SamusCycles {STANDLEFT, STANDRIGHT, RUNLEFT, RUNRIGHT};

void tile2world(float* worldX, float* worldY, int tileCol, int tileRow,
	float offsetX, float offsetY);
void world2tile(float worldX, float worldY, int* tileCol, int* tileRow,
	float offsetX, float offsetY);
float tileCollide(float x, float y, float v, float h, bool isY,
	float offsetX, float offsetY, int** level);

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
	
	int** level; int width, height; float offsetX, offsetY;

	void fillLevel(const char* fname);
	void fillEntities();

	void physics();

	void movePlayer(float elapsed);
	StateAndRun updateGame(float elapsed);
	
	void renderGame();
	void drawLevel();
};