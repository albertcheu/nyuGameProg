#include "GameClass.h"

GameClass::~GameClass(){ SDL_Quit(); }
GameClass::GameClass()
	: whichPB(PB1), whichEB(EB1), state(GAME), nextShift(-0.02f), lastTickCount(0) {
	//Boilerplate
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("SPACE INVADERS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//Establish how big our screen is
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	//This is a 2D game, so it ought to be orthographic
	//Adjust the scale (ought to fit the screen!)
	glOrtho(-UNIT_WIDTH, UNIT_WIDTH, -UNIT_HEIGHT, UNIT_HEIGHT, -1, 1);

	//Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	OutputDebugString("Finished setup");

	fillEntities();
}

void GameClass::fillEntities(){
	//Objects are all in one 1024x1024 sheet
	TextureData td = LoadTextureRGBA("sheet.png");
	int swidth = td.width;
	int sheight = td.height;
	
	//Six player beams: x="856" y="869" width="9" height="57"
	for (int i = 0; i < 6; i++){
		entities.push_back(Entity(0,0,0.01f, 0.01f*57.0f/9.0f,
			Sprite(td.id, 856.0f / swidth, 869.0f/sheight, 9.0f/swidth, 57.0f/sheight),
			false));
	}

	//Player: x="211" y="941" width="99" height="75"
	Sprite playerSprite(td.id, 211.0f / swidth, 941.0f / sheight, 99.0f / swidth, 75.0f / sheight);
	Entity player(0, -0.6f, SHIP_WIDTH, SHIP_WIDTH*75.0f / 99.0f, playerSprite);
	entities.push_back(player);

	//Four walls
	Entity top(0, 1, 2 * UNIT_WIDTH, 0.12f, Sprite(), false);
	Entity bot(0, -1, 2 * UNIT_WIDTH, 0.12f, Sprite(), false);
	Entity left(-UNIT_WIDTH, 0, 0.12f, 2 * UNIT_HEIGHT, Sprite(), false);
	Entity right(UNIT_WIDTH, 0, 0.12f, 2 * UNIT_HEIGHT, Sprite(), false);
	entities.push_back(top); entities.push_back(bot); entities.push_back(left); entities.push_back(right);

	//Eight enemy beams: x="855" y="173" width="9" height="57"
	for (int i = 0; i < 10; i++){
		entities.push_back(Entity(0, 0, 0.01f, 0.01f*57.0f / 9.0f,
			Sprite(td.id, 855.0f / swidth, 173.0f / sheight, 9.0f / swidth, 57.0f / sheight),
			false));
	}

	//Three enemy types: here are their uv coordinates and dimensions
	float ex[3] = { 423.0f, 144.0f, 518.0f }; float ey[3] = { 728.0f, 156.0f, 325.0f };
	float ew[3] = { 93.0f, 103.0f, 82.0f }; //height is always 84pix
	for (int i = 0; i < ENEMIES; i++){
		Sprite enemySprite(td.id, ex[i%3] / swidth, ey[i%3] / sheight, ew[i%3] / swidth, 84.0f / sheight);
		Entity enemy(0, 0, SHIP_WIDTH, SHIP_WIDTH*84.0f / ew[i % 3], enemySprite
			//,false);
			);
		entities.push_back(enemy);
	}

	//Center of the enemy box
	Entity center(-0.14f, 0, 0.05f, 0.05f, Sprite()
		,false);
		//);
	center.setXspeed(0.08f);
	entities.push_back(center);
}

void GameClass::moveBeams(float elapsed){
	for (unsigned i = PB1; i <= PB6; i++){
		float speed = entities[i].getYspeed();
		if (!speed) { continue; }
		entities[i].setY(entities[i].getY() + elapsed*speed);
		if (entities[i].collide(entities[TOP])){ entities[i].reset(); }
	}
}
void GameClass::movePlayer(float elapsed){
	//Poll for arrow key
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT]){ entities[PLAYER].setX(entities[PLAYER].getX() - elapsed*SHIP_SPEED); }
	if (keys[SDL_SCANCODE_RIGHT]){ entities[PLAYER].setX(entities[PLAYER].getX() + elapsed*SHIP_SPEED); }

	//Player hit walls
	if (entities[PLAYER].collide(entities[LEFT]))
	{
		entities[PLAYER].setX(-UNIT_WIDTH + entities[LEFT].getHalfWidth() + entities[PLAYER].getHalfWidth());
	}
	else if (entities[PLAYER].collide(entities[RIGHT]))
	{
		entities[PLAYER].setX(UNIT_WIDTH - entities[RIGHT].getHalfWidth() - entities[PLAYER].getHalfWidth());
	}
}
void GameClass::moveEnemies(float elapsed){
	//First move the center
	int center = entities.size() - 1;
	float x = entities[center].getX();
	float y = entities[center].getY();
	float xSpeed = entities[center].getXspeed();
	float ySpeed = entities[center].getYspeed();
	//We've moved down far enough
	if (xSpeed == 0 && y < nextShift){
		xSpeed = 0.08f * (x < 0 ? 1 : -1);
		ySpeed = 0;
		nextShift -= 0.02f;
	}
	//We've moved horizontally far enough
	else if (ySpeed == 0 && (x <= -0.14f || x >= 0.14f)){
		ySpeed = -0.02f;
		xSpeed = 0;
	}
	entities[center].setX(x + elapsed*xSpeed);
	entities[center].setY(y + elapsed*ySpeed);
	entities[center].setXspeed(xSpeed);
	entities[center].setYspeed(ySpeed);
	
	//Then move enemies relative to center
	for (unsigned i = ESTART; i < ESTART + ENEMIES; i++){
		entities[i].setY(entities[center].getY() + dy[(i - ESTART) % 3]);
		entities[i].setX(entities[center].getX() + dx[(i - ESTART) / 3]);
		//Game over when hit bottom or player!
	}
}

bool GameClass::run(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastTickCount;
	lastTickCount = ticks;
	bool ans = true;
	if (state == GAME){
		ans = updateGame(elapsed);
		renderGame();
	}
	else if (state == PAUSE){
		ans = updatePause(elapsed);
		renderPause();
		
	}
	return ans;
}

bool GameClass::updateGame(float elapsed){
	SDL_Event event;
	//Enable closing of window
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return false;
		}
		else if (event.type == SDL_KEYDOWN){
			//If we press escape, pause
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){ state = PAUSE; }
			else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
				//Fire!				
				entities[whichPB].setVisibility(true);
				entities[whichPB].setPos(entities[PLAYER].getX(), entities[PLAYER].getY());
				entities[whichPB].setYspeed(BEAM_SPEED);
				whichPB++;
				if (whichPB > PB6) { whichPB = PB1; }
			}
		}
	}

	moveBeams(elapsed);
	movePlayer(elapsed);
	moveEnemies(elapsed);

	return true;
}

void GameClass::renderGame(){
	glClear(GL_COLOR_BUFFER_BIT);
	for (size_t i = 0; i < entities.size(); i++){ entities[i].draw(); }
	SDL_GL_SwapWindow(displayWindow);
}

bool GameClass::updatePause(float elapsed){
	SDL_Event event;
	//Enable closing of window
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return false;
		}
		else if (event.type == SDL_KEYDOWN){
			//If we press escape, pause
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){ state = GAME; }
		}
	}
	return true;
}
void GameClass::renderPause(){
	glClear(GL_COLOR_BUFFER_BIT);
	
	//Draw fancy pause screen

	SDL_GL_SwapWindow(displayWindow);
}