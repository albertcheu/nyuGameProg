#include "GameClass.h"
#include "Helpers.h"

GameClass::~GameClass(){ SDL_Quit(); }
GameClass::GameClass()
	: whichPB(PB1), whichEB(EB1), health(HEALTH), numAlive(ENEMIES),
	state(GAME), nextShift(-0.02f), lastTickCount(0), shouldFire(false) {
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
	glOrtho(-UNIT_WIDTH, UNIT_WIDTH, -UNIT_HEIGHT, UNIT_HEIGHT, -1, 1);
	//Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	td = LoadTextureRGBA("sheet.png");
	fillEntities();
}

void GameClass::restartGame(){
	nextShift = -0.02f; health = HEALTH; numAlive = ENEMIES;
	whichEB = EB1; whichPB = PB1; shouldFire = false;
	//reset beams
	for (unsigned i = PB1; i <= PB6; i++){ entities[i].reset(); }
	for (unsigned i = EB1; i <= EB10; i++){ entities[i].reset(); }
	for (unsigned i = 0; i < ENEMIES; i++){ entities[ESTART + i].setVisibility(true); }
	entities.back().setPos(0, -0.14f);
	entities.back().setSpeed(0.08f,0);
}

void GameClass::fillEntities(){
	int swidth = td.width; int sheight = td.height;
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
	//Ten enemy beams: x="855" y="173" width="9" height="57"
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
	//Center of the grid of enemies
	Entity center(-0.14f, 0, 0.05f, 0.05f, Sprite()
		,false);
		//);
	center.setXspeed(0.08f);
	entities.push_back(center);
}

void GameClass::movePlayerBeams(float elapsed){
	for (unsigned i = PB1; i <= PB6; i++){
		float speed = entities[i].getYspeed();
		if (!speed) { continue; }
		entities[i].setY(entities[i].getY() + elapsed*speed);
		if (entities[i].collide(entities[TOP])){ entities[i].reset(); }
		
		//For the column of enemies nearest to the beam, check if there is a collision
		float x = entities[i].getX();
		float minDiff = 1.0f; float centerX = entities[entities.size() - 1].getX(); unsigned whichCol = 0;
		for (unsigned j = 0; j < 5; j++){
			float diff = abs((dx[j] + centerX) - x);
			if (diff < minDiff){
				minDiff = diff;
				whichCol = j;
			}
		}
		for (unsigned j = 0; j < 3; j++){
			unsigned loc = ESTART + (whichCol * 3 + j);
			if (entities[i].collide(entities[loc]) && entities[loc].getVisibility()){
				entities[loc].reset();
				entities[i].reset();
				numAlive--;
				break;
			}
		}
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

bool GameClass::moveEnemyBeams(float elapsed){
	for (unsigned i = EB1; i <= EB10; i++){
		float speed = entities[i].getYspeed();
		if (!speed) { continue; }
		entities[i].setY(entities[i].getY() + elapsed*speed);
		if (entities[i].collide(entities[BOT])){ entities[i].reset(); }
		else if (entities[i].collide(entities[PLAYER])){
			//Damage
			health--;
			entities[i].reset();
			if (!health) { return true; }
		}
	}
	return false;
}

bool GameClass::moveEnemies(float elapsed){
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
		shouldFire = true;
	}
	//We've moved horizontally far enough
	else if (ySpeed == 0 && (x <= -0.14f || x >= 0.14f)){
		ySpeed = -0.02f;
		xSpeed = 0;
		shouldFire = true;
	}
	
	entities[center].setXspeed(xSpeed);
	entities[center].setYspeed(ySpeed);
	entities[center].setPos(elapsed);
	
	//Then move enemies relative to center
	for (unsigned i = ESTART; i < ESTART + ENEMIES; i++){
		if (!entities[i].getVisibility()) { continue; }
		entities[i].setY(entities[center].getY() + dy[(i - ESTART) % 3]);
		entities[i].setX(entities[center].getX() + dx[(i - ESTART) / 3]);
		//Game over when hit bottom or player!
		if (entities[i].collide(entities[PLAYER]) || entities[i].collide(entities[BOT])){ return true; }
	}
	return false;
}

void GameClass::enemyFire(){
	if (shouldFire){
		//Bottommost enemies shoot
		for (int i = 0; i < ENEMIES; i+=3){
			for (int j = i + 2; j >= i; j--){
				int whichEnemy = ESTART + j;
				if (entities[whichEnemy].getVisibility()) {
					fireBeam(whichEB, -1, whichEnemy);
					whichEB++;
					if (whichEB > EB10) { whichEB = EB1; }
					break;
				}
			}
		}
		shouldFire = false;
	}
}

bool GameClass::run(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastTickCount;
	lastTickCount = ticks;
	bool ans = true;

	switch (state){
	case GAME:
		ans = updateGame(elapsed);
		renderGame();
		break;
	case PAUSE:
		ans = updatePause(elapsed);
		renderPause();
		break;
	case START:
		break;
	case WIN:
		break;
	case LOSE:
		break;
	}
	
	return ans;
}

void GameClass::fireBeam(unsigned whichBeam, int dir, unsigned whichShip){
	entities[whichBeam].setVisibility(true);
	entities[whichBeam].setPos(entities[whichShip].getX(), entities[whichShip].getY());
	entities[whichBeam].setYspeed(dir*BEAM_SPEED);
}

bool GameClass::updateGame(float elapsed){
	switch (getKey()){
	case UNPRESSED: break;
	case KEY_SPACE:
		fireBeam(whichPB, 1, PLAYER);
		whichPB++;
		if (whichPB > PB6) { whichPB = PB1; }
		break;
	case KEY_ESCAPE:
		state = PAUSE;
		return true;
		break;
	case CLOSE_WINDOW:
		return false;
		break;
	}

	movePlayerBeams(elapsed);
	movePlayer(elapsed);

	if (!numAlive) {
		//won: change state to Win
		restartGame();
	}
	else if (moveEnemyBeams(elapsed) || moveEnemies(elapsed)){
		//lost
		//change state to Game Over
		restartGame();
	}
	else{ enemyFire(); }

	return true;
}

void GameClass::renderGame(){
	glClear(GL_COLOR_BUFFER_BIT);
	for (size_t i = 0; i < entities.size(); i++){ entities[i].draw(); }
	SDL_GL_SwapWindow(displayWindow);
}

bool GameClass::updatePause(float elapsed){
	switch (getKey()){
	case UNPRESSED: break;
	case KEY_SPACE:break;
	case KEY_ESCAPE:
		state = GAME;
		break;
	case CLOSE_WINDOW:
		return false;
		break;
	}
	return true;
}
void GameClass::renderPause(){
	glClear(GL_COLOR_BUFFER_BIT);
	
	//Draw fancy pause screen

	SDL_GL_SwapWindow(displayWindow);
}