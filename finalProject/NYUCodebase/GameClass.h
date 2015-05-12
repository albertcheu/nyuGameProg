#pragma once
#include "Entity.h"
#include "Level.h"
#include "Menu.h"
#include "Generator.h"

#define TIMESTEP 0.0166f
#define MAX_STEPS 6

#define BEAMSPEED 3.5f
#define NUMBEAMS 4
#define NUMPARTICLES 8

#define GRAVITY -7.8f
#define FRIC_Y 5.6f
#define JUMP 4.0f
#define ENEMY_JUMP 3.1f

#define HITSPEED 2.7f

#define FRIC_X 7.7f
#define MOVE 3.8f
#define ENEMY_MOVE 3.0f

#define SHIELD_HEALTH 6

enum SamusCycles {
	STANDLEFT, STANDRIGHT, RUNLEFT, RUNRIGHT,
	SITLEFT, SITRIGHT,
	STANDLEFTUP, STANDRIGHTUP,
	INAIRLEFT, INAIRRIGHT
};

class Samus : public Dynamic{
public:
	Samus();
	Samus(float x, float y, float width, float height, Sprite s, int swidth, int sheight);
	
	bool lookLeft, standing, aimUp;
	void standUp(); void sitDown();
	void nextFrame();
	bool collideBounce(Dynamic& other, float bounceMag);

private:
	std::vector<AnimCycle> cycles;
};

class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:
	Generator g;
	Menu m;
	Mix_Chunk *move, *switchSound, *bump;

	GameState state;
	SDL_Window* displayWindow;
	
	//Doors and pickups are from one file (mfTRO.png)
	TextureData pool;
	Mix_Chunk* pickupSound, *hurtSound, *hitRunner, *hitHopper, *hitDoor, *hitShield;
	std::vector<Pickup> pickups; void createPickups();
	Text healthDisplay, maxHealthDisplay;

	//Load in the sdl and opengl functions, and get the pool
	TextureData loadOpenGL();

	//Contains level data and handles collisions with platforms
	Level theLevel;
	void loadLevel(const char* fname, TextureData texSource);

	Samus* player; void createPlayer();
	Entity hurtFlash; float hurtTime;

	std::vector<Dynamic> enemies;
	Sprite hopperSprite, runnerSprite, flierSprite, bossSprite, bbSprite;
	void createEnemySprites();
	Entity shield; BeamColor shieldRating; int shieldHealth;
	Dynamic* bossBeam; float bossTime;

	//Music to play during the level
	Mix_Music* music;
	
	//Samus can fire four kinds of beams (color-coded)
	std::vector<Beam> beams; size_t whichRed, whichYellow, whichGreen, whichBlue;
	void createBeams();
	void playerShoot(size_t& which, size_t cap);
	std::vector<Dynamic> particles;
	void spawnParticles(size_t which, float x, float y);

	//Doors that open to weapon fire
	std::vector<Door> doors;
	Sprite redDoor, yellowDoor, greenDoor, blueDoor;
	void createDoorSprite(Sprite& d, float u_offset);

	//Time variables
	float lastTickCount, leftover, elapsed, frameChange;

	void physics();
	void moveDynamicY(Dynamic& d);
	void moveDynamicX(Dynamic& d);
	void moveDynamic(Dynamic& d);
	void moveEnemy(Dynamic& d);
	void moveHoriz(Dynamic& d);
	bool castToPlayer(Dynamic& d);
	void weakenShield();

	GameState handleEvents();
	void pollForPlayer();
	void animate();
	
	void renderGame();

	bool inScreen(Entity& subject);
};