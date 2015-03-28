#pragma once
#include "Entity.h"
#include "Level.h"

#define TIMESTEP 0.0166f
#define MAX_STEPS 6

#define BEAMSPEED 3.5f
#define NUMBEAMS 10

#define GRAVITY -7.8f
#define FRIC_Y 5.6f
#define JUMP 4.0f

#define FRIC_X 7.7f
#define MOVE 3.8f

#define PLAYER 0
enum SamusCycles {STANDLEFT, STANDRIGHT, RUNLEFT, RUNRIGHT};

void moveDynamicY(Dynamic& d, Level& theLevel);
void moveDynamicX(Dynamic& d, Level& theLevel);

class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:
	SDL_Window* displayWindow;
	
	//Doors and pickups are from one file (mfTRO.png)
	TextureData pool;
	Mix_Chunk* pickupSound;
	std::vector<Pickup> pickups; void createPickups();

	//Load in the sdl and opengl functions, and get the pool
	TextureData loadOpenGL();

	//Contains level data and handles collisions with platforms
	Level theLevel;
	void loadLevel(const char* fname, TextureData texSource);

	//Music to play during the level
	Mix_Music* music;
	
	//Samus can fire four kinds of beams (color-coded)
	std::vector<Beam> beams; size_t whichRed, whichYellow, whichGreen, whichBlue;
	void createBeams();
	void playerShoot(size_t& which, size_t cap);

	//Doors that open to weapon fire
	std::vector<Door> doors;
	Sprite redDoor, yellowDoor, greenDoor, blueDoor;//(avoid repeated constructor calls)
	void createDoorSprite(Sprite& d, float u_offset);

	//Player and enemies
	TextureData spriteSheet; void createPlayer();
	std::vector<Dynamic> dynamics; Dynamic* player;
	Sprite hopperSprite, runnerSprite;	void createEnemySprites();

	//Player's animation cycles; are we looking left or right
	std::vector<AnimCycle> cycles; bool lookLeft;

	//Time variables for physics and animation
	float lastTickCount, leftover, elapsed, frameChange;

	void physics();

	StateAndRun handleEvents();
	void pollForPlayer();
	void animatePlayer();
	
	void renderGame();
};