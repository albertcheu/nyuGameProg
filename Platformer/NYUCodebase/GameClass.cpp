#include "GameClass.h"
GameClass::~GameClass(){ SDL_Quit(); }
GameClass::GameClass()
	: lastTickCount(0), leftover(0), player(NULL), lookLeft(true), frameChange(0) {
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
	
	sheet = LoadTextureRGBA("superPowerSuit.png");
	
	fillEntities();
}

void GameClass::fillEntities(){
	int swidth = sheet.width; int sheight = sheet.height;
	cycles.push_back(AnimCycle(swidth, sheight, 25, 42, 3, 1));
	cycles.push_back(AnimCycle(swidth, sheight, 25, 42, 3, 1, 3));
	cycles.push_back(AnimCycle(swidth, sheight, 35, 41, 10, 3));
	cycles.push_back(AnimCycle(swidth, sheight, 35, 41, 10, 7));

	//play as Samus
	//26x46, 11th row
	//Sprite p(sheet.id, 17.0f / swidth, 21.0f / sheight, 25.0f / swidth, 42.0f / sheight);
	float y = 64.0f * 11 - 46; float x = (64.0f - 26.0f) / 2;
	Sprite p(sheet.id, x / swidth, y / sheight, 26.0f / swidth, 46.0f / sheight);
	//float playerHeight = 0.07f*46.0f / 26.0f;
	//float playerWidth = 0.07f;
	float playerHeight = 0.12f;
	float playerWidth = playerHeight*26.0f/46.0f;
	dynamics.push_back(Dynamic(0, -0.56f, playerWidth, playerHeight, p));

	//pickup the morph balls
	Sprite s(sheet.id, 24.0f / swidth, 1264.0f / sheight, 15.0f / swidth, 15.0f / sheight);
	dynamics.push_back(Dynamic(0, -0.2f, 0.05f, 0.05f, s));
	dynamics.push_back(Dynamic(-0.5f, 0.2f, 0.05f, 0.05f, s));
	dynamics.push_back(Dynamic(0.5f, 0.2f, 0.05f, 0.05f, s));

	player = &dynamics[PLAYER];//do last: the vector's location in memory changes after pushes

	//floor
	statics.push_back(Entity(0, -0.66f, 1.5f, 0.05f, Sprite()));

	//walls
	statics.push_back(Entity(-0.775f, 0, 0.05f, 1.32f, Sprite()));
	statics.push_back(Entity(0.775f, 0, 0.05f, 1.32f, Sprite()));

	//ceiling
	statics.push_back(Entity(0, 0.7f, 1.5f, 0.05f, Sprite()));

	//platforms
	statics.push_back(Entity(-0.5f, -0.45f, 0.5f, 0.05f, Sprite()));
	statics.push_back(Entity(0.5f, -0.45f, 0.5f, 0.05f, Sprite()));
	statics.push_back(Entity(0, -0.25f, 0.4f, 0.05f, Sprite()));//middle
	statics.push_back(Entity(-0.5f, -0.05f, 0.5f, 0.05f, Sprite()));
	statics.push_back(Entity(0.5f, -0.05f, 0.5f, 0.05f, Sprite()));
	statics.push_back(Entity(-0.5f, 0.15f, 0.3f, 0.05f, Sprite()));
	statics.push_back(Entity(0.5f, 0.15f, 0.3f, 0.05f, Sprite()));
}

void GameClass::movePlayer(float elapsed){
	//Poll for arrow key
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	SpriteFrame sf;
	if (keys[SDL_SCANCODE_LEFT]) {
		lookLeft = true;
		player->setAx(-MOVE);
		sf = cycles[RUNLEFT].getNext();
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		lookLeft = false;
		player->setAx(MOVE);
		sf = cycles[RUNRIGHT].getNext();
	}
	else {
		player->setAx(0);
		if (lookLeft) { sf = cycles[STANDLEFT].getNext(); }
		else { sf = cycles[STANDRIGHT].getNext(); }
	}
	if (lastTickCount - frameChange >= 0.06f){
		frameChange = lastTickCount;
		player->setFrame(sf);
	}	
}

StateAndRun GameClass::updateGame(float elapsed){
	StateAndRun ans = { 0, true };

	//Keyboard (and close-window) events
	switch (getKey()){
	case KEY_SPACE:
		if (player->getBottom()) { player->setVy(JUMP); }
		break;
	case KEY_ESCAPE:
		ans = { 0, false };
		break;
	case CLOSE_WINDOW:
		ans = { 0, false };
		break;
	case OTHER: break;
	}

	movePlayer(elapsed);

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
	}
	for (size_t i = PLAYER + 1; i < dynamics.size(); i++){
		//Pickups disappear
		if (dynamics[i].getVisibility() && dynamics[PLAYER].collide(dynamics[i])) {
			dynamics[i].reset();
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
	StateAndRun sar = updateGame(elapsed);
	renderGame();
	SDL_GL_SwapWindow(displayWindow);
	
	return sar.keepRunning;
}

void GameClass::renderGame(){
	for (size_t i = 0; i < statics.size(); i++){ statics[i].draw(); }
	for (size_t i = 0; i < dynamics.size(); i++){ dynamics[i].draw(); }
}