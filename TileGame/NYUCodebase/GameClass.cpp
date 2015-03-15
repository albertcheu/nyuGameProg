#include "GameClass.h"

void tile2world(float* worldX, float* worldY, int tileCol, int tileRow,
	float offsetX, float offsetY){
	//Center of tile
	*worldX = TILEUNITS*tileCol + offsetX + TILEUNITS / 2;
	*worldY = -TILEUNITS*tileRow + offsetY - TILEUNITS / 2;
}

void world2tile(float worldX, float worldY, int*tileCol , int*tileRow,
	float offsetX, float offsetY){
	*tileCol = (int)floor((worldX - offsetX) / TILEUNITS);
	*tileRow = (int)floor((worldY - offsetY) / -TILEUNITS);
}

GameClass::~GameClass(){
	freeLevel(height, &level);
	SDL_Quit();
}
GameClass::GameClass()
	: lastTickCount(0), leftover(0), player(NULL), lookLeft(true), frameChange(0),
	level(NULL), width(0), height(0), offsetX(0), offsetY(0)
	{
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
	
	tileMap = LoadTextureRGB("mfTRO.jpg");
	spriteSheet = LoadTextureRGBA("superPowerSuit.png");
	
	fillLevel("levelOne.txt");
	fillEntities();
}

void GameClass::fillLevel(const char* fname){
	if (level){ freeLevel(height, &level); }
	loadLevel(fname, &width, &height, &level);
	offsetX = -TILEUNITS * width / 2; offsetY = TILEUNITS * height / 2;
	OutputDebugString((std::to_string(offsetX) + ' ' + std::to_string(offsetY)).c_str());
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (!level[y][x]) { continue; }

			float u = (float)(((int)level[y][x]) % TILECOUNTX) / (float)TILECOUNTX;
			float v = (float)(((int)level[y][x]) / TILECOUNTX) / (float) TILECOUNTY;
			tileVerts.insert(tileVerts.end(), {
				TILEUNITS * x, -TILEUNITS * y,
				TILEUNITS * x, (-TILEUNITS * y) - TILEUNITS,
				(TILEUNITS * x) + TILEUNITS, (-TILEUNITS * y) - TILEUNITS,
				(TILEUNITS * x) + TILEUNITS, -TILEUNITS * y
			});
			float spriteWidth = 1.0f / (float)TILECOUNTX;
			float spriteHeight = 1.0f / (float)TILECOUNTY;
			tileTexts.insert(tileTexts.end(), { u, v,
				u, v + (spriteHeight),
				u + spriteWidth, v + (spriteHeight),
				u + spriteWidth, v
			});
		}
	}
}

void GameClass::fillEntities(){
	int swidth = spriteSheet.width; int sheight = spriteSheet.height;
	cycles.push_back(AnimCycle(swidth, sheight, 25, 42, 3, 1));
	cycles.push_back(AnimCycle(swidth, sheight, 25, 42, 3, 1, 3));
	cycles.push_back(AnimCycle(swidth, sheight, 35, 41, 10, 3));
	cycles.push_back(AnimCycle(swidth, sheight, 35, 41, 10, 7));

	//play as Samus
	//26x46, 11th row
	//Sprite p(sheet.id, 17.0f / swidth, 21.0f / sheight, 25.0f / swidth, 42.0f / sheight);
	float y = 64.0f * 11 - 46; float x = (64.0f - 26.0f) / 2;
	Sprite p(spriteSheet.id, x / swidth, y / sheight, 26.0f / swidth, 46.0f / sheight);
	float playerHeight = 0.17f;
	float playerWidth = playerHeight*26.0f/46.0f;
	dynamics.push_back(Dynamic(0, 0, playerWidth, playerHeight, p));

	player = &dynamics[PLAYER];//do last: the vector's location in memory changes after pushes

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

float tileCollide(float x, float y, float v, float h, bool isY,
	float offsetX, float offsetY, int** level){
	int tileCol, tileRow;
	world2tile(x, y, &tileCol, &tileRow, offsetX, offsetY);

	float tileX, tileY;
	if (//tileCol > -1 && tileRow > -1 && tileCol < width && tileRow < height &&
		level[tileRow][tileCol] > 97) {
		tile2world(&tileX, &tileY, tileCol, tileRow, offsetX, offsetY);
		return depenetrate(v, h, isY ? tileY : tileX, TILEUNITS / 2);
	}
	return v;
}

void GameClass::physics(){
	//For every dynamic object...
	for (size_t i = 0; i < dynamics.size(); i++){
		dynamics[i].noTouch();
		
		//Move in y
		dynamics[i].setVy(lerp(dynamics[i].getVy(), 0, TIMESTEP*FRIC_Y));
		dynamics[i].bumpVy(dynamics[i].getAy() * TIMESTEP + GRAVITY * TIMESTEP);
		dynamics[i].bumpY(dynamics[i].getVy()*TIMESTEP);

		float x = dynamics[i].getX(); float y = dynamics[i].getY();

		//resolve collisions
		float hh = dynamics[i].getHalfHeight();

		float newY = tileCollide(x, y - hh, y, hh, true, offsetX, offsetY, level);
		if (newY != y){ dynamics[i].stickBottom(newY); }

		newY = tileCollide(x, y + hh, y, hh, true, offsetX, offsetY, level);
		if (newY != y){ dynamics[i].stickTop(newY); }

		//Move in x and resolve collisions
		dynamics[i].setVx(lerp(dynamics[i].getVx(), 0, TIMESTEP*FRIC_X));
		dynamics[i].bumpVx(dynamics[i].getAx() * TIMESTEP);
		dynamics[i].bumpX(dynamics[i].getVx() * TIMESTEP);
		float hw = dynamics[i].getHalfWidth(); float quart = dynamics[i].getHeight() / 4.0f;
		for (int j = 1; j < 4; j++){
			float newX = tileCollide(x - hw, y - hh + quart*j, x, hw, false, offsetX, offsetY, level);
			if (newX != x) { dynamics[i].stickLeft(newX); }

			newX = tileCollide(x + hw, y - hh + quart*j, x, hw, false, offsetX, offsetY, level);
			if (newX != x) { dynamics[i].stickRight(newX); }
		}
		
	}
}

bool GameClass::run(){
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
	
	StateAndRun sar = updateGame(elapsed);
	
	renderGame();
	
	return sar.keepRunning;
}

void GameClass::drawLevel(){
	glPushMatrix();
	glTranslatef(-player->getX(), -player->getY(), 0.0f);

	glTranslatef(offsetX, offsetY, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tileMap.id);
	glVertexPointer(2, GL_FLOAT, 0, tileVerts.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, tileTexts.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, tileVerts.size() * 4);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void GameClass::renderGame(){
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	for (size_t i = 0; i < dynamics.size(); i++){
		dynamics[i].draw(-player->getX(), -player->getY());
	}
	
	drawLevel();

	SDL_GL_SwapWindow(displayWindow);
}