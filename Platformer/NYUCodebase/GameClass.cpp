#include "GameClass.h"
GameClass::~GameClass(){ SDL_Quit(); }
GameClass::GameClass(): lastTickCount(0), leftover(0) {
	//Boilerplate
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Dope Platformer",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	//Establish how big our screen is
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	//This is a 2D game, so it ought to be orthographic
	glOrtho(-UNIT_WIDTH, UNIT_WIDTH, -UNIT_HEIGHT, UNIT_HEIGHT, -1, 1);
	//Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	sheet = LoadTextureRGBA("JnRTiles.png");
	fillEntities();
}

void GameClass::fillEntities(){
	int swidth = sheet.width; int sheight = sheet.height;
	//player
	dynamics.push_back(Entity(0, -0.56, 0.05f, 0.05f, Sprite()));

	//pickups (13x13)
	Sprite s(sheet.id, (swidth-22.0f)/swidth, 10.0f/sheight, 15.0f / swidth, 15.0f / sheight);
	dynamics.push_back(Entity(0, -0.2f, 0.05f, 0.05f, s));

	//floor
	statics.push_back(Entity(0, -0.66f, 1.5f, 0.05f, Sprite()));

	//walls
	statics.push_back(Entity(-0.775f, 0, 0.05f, 1.32f, Sprite()));
	statics.push_back(Entity(0.775f, 0, 0.05f, 1.32f, Sprite()));

	//ceiling
	statics.push_back(Entity(0, 0.8f, 1.5f, 0.05f, Sprite()));

	//platforms
	statics.push_back(Entity(-0.5f, -0.45f, 0.5f, 0.05f, Sprite()));
	statics.push_back(Entity(0.5f, -0.45f, 0.5f, 0.05f, Sprite()));
	statics.push_back(Entity(0, -0.25f, 0.4f, 0.05f, Sprite()));
	statics.push_back(Entity(-0.5f, -0.05f, 0.5f, 0.05f, Sprite()));
	statics.push_back(Entity(0.5f, -0.05f, 0.5f, 0.05f, Sprite()));
}

void GameClass::movePlayer(){
	//Poll for arrow key
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT]) { dynamics[PLAYER].setAx(-MOVE); }
	else if (keys[SDL_SCANCODE_RIGHT]) { dynamics[PLAYER].setAx(MOVE); }
	else { dynamics[PLAYER].setAx(0); }
}

StateAndRun GameClass::updateGame(){
	StateAndRun ans = { 0, true };
	switch (getKey()){
	case KEY_SPACE:
		if (dynamics[0].getBottom()) { dynamics[0].setVy(JUMP); }
		break;
	case KEY_ESCAPE:
		ans = { 0, false };
		break;
	case CLOSE_WINDOW:
		ans = { 0, false };
		break;
	case OTHER: break;
	}

	movePlayer();

	return ans;
}

void GameClass::physics(){
	//For every dynamic object...
	for (size_t i = 0; i < dynamics.size(); i++){
		dynamics[i].noTouch();
		//Move in y and resolve collisions
		dynamics[i].setVy(lerp(dynamics[i].getVy(), 0, TIMESTEP*FRIC_Y));
		dynamics[i].bumpVy(dynamics[i].getAy() * TIMESTEP + GRAVITY * TIMESTEP);
		dynamics[i].bumpY(dynamics[i].getVy()*TIMESTEP);

		for (size_t j = 0; j < statics.size(); j++){
			if (dynamics[i].collide(statics[j])){
				//De-penetrate
				dynamics[i].setY(depenetrate(dynamics[i].getY(), dynamics[i].getHalfHeight(),
					statics[j].getY(), statics[j].getHalfHeight()));
				if (dynamics[i].getY() > statics[j].getY()) { dynamics[i].setBottom(); }
				else { dynamics[i].setTop(); }
				//Set vy to 0
				dynamics[i].setVy(0);
			}
		}

		//Move in x and resolve collisions
		dynamics[i].setVx(lerp(dynamics[i].getVx(), 0, TIMESTEP*FRIC_X));
		dynamics[i].bumpVx(dynamics[i].getAx() * TIMESTEP);
		dynamics[i].bumpX(dynamics[i].getVx() * TIMESTEP);
		for (size_t j = 0; j < statics.size(); j++){
			if (dynamics[i].collide(statics[j])){
				//De-penetrate
				dynamics[i].setX(depenetrate(dynamics[i].getX(), dynamics[i].getHalfWidth(),
					statics[j].getX(), statics[j].getHalfWidth()));
				if (dynamics[i].getX() > statics[j].getX()) { dynamics[i].setLeft(); }
				else { dynamics[i].setRight(); }
				//Set vx to 0
				dynamics[i].setVx(0);
			}
		}

		for (size_t j = PLAYER + 1; j < dynamics.size(); j++){
			//Pickups disappear
			if (dynamics[j].getVisibility() && dynamics[PLAYER].collide(dynamics[j])) {
				dynamics[j].reset();
			}
		}
	}
}

bool GameClass::run(){
	glClear(GL_COLOR_BUFFER_BIT);

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastTickCount;
	lastTickCount = ticks;
	
	float fixedElapsed = elapsed + leftover;
	if (fixedElapsed > TIMESTEP * MAX_STEPS) { fixedElapsed = TIMESTEP * MAX_STEPS;	}
	while (fixedElapsed >= TIMESTEP) {
		fixedElapsed -= TIMESTEP;
		physics();
	}
	leftover = fixedElapsed;
	//variable update = animation

	StateAndRun sar = updateGame();
	renderGame();
	SDL_GL_SwapWindow(displayWindow);
	
	return sar.keepRunning;
}

void GameClass::renderGame(){
	for (size_t i = 0; i < statics.size(); i++){ statics[i].draw(); }
	for (size_t i = 0; i < dynamics.size(); i++){ dynamics[i].draw(); }
}