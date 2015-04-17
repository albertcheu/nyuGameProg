#pragma once
#include "Entity.h"
#include "Level.h"

#define TIMESTEP 0.0166f
#define MAX_STEPS 6

#define BEAMSPEED 3.5f
#define NUMBEAMS 10

#define FRIC 5.0f
#define MOVE 3.8f
#define TURN_SPEED 100
#define ASTEROID_ACCEL 0.4f

#define PLAYER 0

enum GameState { START, PLAY, LOSE, WIN, QUIT };

class GameClass{
public:
	GameClass();
	~GameClass();
	bool run();

private:
	SDL_Window* displayWindow;
	
	//Load in the sdl and opengl functions, and get the pool
	TextureData loadOpenGL();

	std::vector<Beam> beams; size_t which; Mix_Chunk* fire, *hitRock;
	void createBeams();
	void playerShoot();

	void resetGame();

	//Player and asteroids
	TextureData spriteSheet; int swidth, sheight;
	void createPlayer(); void createAsteroids();
	std::vector<Dynamic> dynamics; Dynamic* player;

	//Coordinates of where to put (white) dots
	std::vector<float> stars; std::vector<float> starColors;
	void createStars();

	Text stateText; GameState state;
	
	Text asteroidCountText; int asteroidCount;
	void createText();

	//Time variables for physics and animation
	float lastTickCount, leftover, elapsed, frameChange;

	void physics();

	StateAndRun handleEvents();
	void pollForPlayer();
	
	void renderGame();
	void drawStars(); void drawAiming();
};