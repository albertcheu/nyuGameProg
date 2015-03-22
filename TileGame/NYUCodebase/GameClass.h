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

class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:
	TextureData loadOpenGL();
	SDL_Window* displayWindow;

	//Contains level data and handles collisions with platforms
	Level theLevel;

	//Player
	TextureData spriteSheet;
	void createPlayer();

	//Doors and pickups are from one file (mfTRO.png)
	TextureData pool;
	
	//What we can shoot is determined by pickups
	std::vector<Pickup> pickups;
	void createPickups();

	std::vector<Beam> beams; size_t whichRed, whichYellow, whichGreen, whichBlue;
	void createBeams();
	void playerShoot(size_t& which, size_t cap);

	//Doors that open to weapon fire
	std::vector<Door> doors;
	Sprite redDoor, yellowDoor, greenDoor, blueDoor;
	void createDoorSprite(Sprite& d, float u_offset);

	//Player and enemies
	std::vector<Dynamic> dynamics; Dynamic* player;
		
	//Player's animation cycles; are we looking left or right
	std::vector<AnimCycle> cycles; bool lookLeft;

	//Time variables for physics and animation
	float lastTickCount, leftover, frameChange;
	
	void loadLevel();

	void physics();

	void pollForPlayer(float elapsed);
	StateAndRun userInput(float elapsed);
	
	void renderGame();
};