#include "GameClass.h"

GameClass::~GameClass(){
	Mix_FreeChunk(fire);
	Mix_FreeChunk(hitRock);
	//OutputDebugString("Freed beam sounds");

	SDL_Quit();
	//OutputDebugString("Quit sdl");
}
TextureData GameClass::loadOpenGL(){
	//Boilerplate
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Asteroids",
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
	
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	return LoadTextureRGBA("sheet.png");
}
GameClass::GameClass()
	: lastTickCount(0), leftover(0), elapsed(0), fire(NULL), hitRock(NULL),
	player(NULL), frameChange(0), which(0), state(START),
	spriteSheet(loadOpenGL()){
	
	swidth = spriteSheet.width; sheight = spriteSheet.height;
	
	createPlayer();
	createBeams();
	
	srand(SDL_GetTicks());
	resetGame();
}

void GameClass::resetGame(){
	stars.clear(); starColors.clear();
	while (dynamics.size() > 1) { dynamics.pop_back(); }
	createAsteroids();
	player = &(dynamics[PLAYER]);
	createStars();
	createText();
}

void GameClass::createPlayer(){
	float playerHeight = 0.17f; float ratio = 99.0f / 75;
	//<SubTexture name="playerShip1_blue.png" x="211" y="941" width="99" height="75"/>
	Sprite p(spriteSheet.id, 211.0f/swidth, 941.0f/sheight, 99.0f / swidth, 75.0f / sheight);
	dynamics.push_back(Dynamic(0, 0, playerHeight*ratio, playerHeight, p, NOT_ENEMY));

	
}

void GameClass::createAsteroids(){
	hitRock = Mix_LoadWAV("beamRock.wav");
	Sprite a1(spriteSheet.id, 407.0f / swidth, 234.0f / sheight, 28.0f / swidth, 28.0f / sheight);
	Sprite a2(spriteSheet.id, 651.0f / swidth, 447.0f / sheight, 43.0f / swidth, 43.0f / sheight);

	int numAsteroids = 4 + (rand() % 3);
	for (int i = 0; i < numAsteroids; i++){
		float percentX = (rand() % 100) / 100.0f;
		float percentY = (rand() % 100) / 100.0f;
		float x = percentX*UNIT_WIDTH * 2; float y = percentY*UNIT_HEIGHT * 2;
		x = (x > UNIT_WIDTH ? x - UNIT_WIDTH : -x);
		y = (y > UNIT_HEIGHT ? y - UNIT_HEIGHT : -y);
		if (rand() % 2 == 0){ dynamics.push_back(Dynamic(x, y, 0.15f, 0.15f, a1, NOT_ENEMY)); }
		else { dynamics.push_back(Dynamic(x, y, 0.2f, 0.2f, a2, NOT_ENEMY)); }
		float angle = (float)(rand() % 360);
		dynamics.back().setAngle(angle);
		dynamics.back().setAx(ASTEROID_ACCEL * cos(angle*M_PI / 180.0f));
		dynamics.back().setAy(ASTEROID_ACCEL * sin(angle*M_PI / 180.0f));
	}
}

void GameClass::createBeams(){
	TextureData btd = LoadTextureRGB("beams.png");
	//Color
	BeamColor bc = RED;
	//Sprite
	Sprite b(btd.id, 0, 0, 1.0f, 0.25f);
	//Sound file
	fire = Mix_LoadWAV("beam1.wav");
	//Actually make the beams now
	for (int j = 0; j < NUMBEAMS; j++){
		beams.push_back(Beam(0.03f, 0.01f, b, bc, fire));
	}
}

void GameClass::createStars(){
	int numStars = 50 + (rand() % 10);
	for (int i = 0; i < numStars; i++){
		float percentX = (rand() % 100) / 100.0f;
		float percentY = (rand() % 100) / 100.0f;
		float x = percentX*UNIT_WIDTH * 4; float y = percentY*UNIT_HEIGHT * 4;
		stars.push_back(x>UNIT_WIDTH*2 ? x - 2*UNIT_WIDTH : -x);
		stars.push_back(y>UNIT_HEIGHT*2 ? y - 2*UNIT_HEIGHT : -y);

		//RGBA
		starColors.push_back(1.0f); starColors.push_back(1.0f); starColors.push_back(1.0f);
		starColors.push_back(1.0f);
	}
}

void GameClass::createText(){
	TextureData font = LoadTextureRGBA("font.png");
	asteroidCount = (int)dynamics.size() - 1;
	std::string s = "# left:" + std::to_string(asteroidCount);
	asteroidCountText = Text(font.id, 1.0f / 16.0f, 0.1f, 0, UNIT_HEIGHT-0.1f, s);

	stateText = Text(font.id, 1.0f / 16.0f, 0.1f, 0, 0, "Press space!");
}

void GameClass::pollForPlayer(){
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_UP]) {
		//Project MOVE onto x and y
		float ax, ay;
		ax = MOVE * cos(player->getAngle()*M_PI / 180.0f + (M_PI / 2));
		ay = MOVE * sin(player->getAngle()*M_PI / 180.0f + (M_PI / 2));
		player->setAx(ax); player->setAy(ay);
	}
	else { player->setAx(0); player->setAy(0); }

	if (keys[SDL_SCANCODE_LEFT]) { player->setAngle(player->getAngle() + TURN_SPEED*elapsed); }
	if (keys[SDL_SCANCODE_RIGHT]) { player->setAngle(player->getAngle() - TURN_SPEED*elapsed); }
}

void GameClass::playerShoot(){
	beams[which].fire(player->getX(), player->getY(), 0);
	beams[which].setAngle(player->getAngle() + 90.0f);
	which++;
	if (which == NUMBEAMS) { which = 0; }

}

StateAndRun GameClass::handleEvents(){
	StateAndRun ans = { state, true };

	//Keyboard (and close-window) events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			ans = { QUIT, false };
		}
		else if (event.type == SDL_KEYDOWN){
			switch (event.key.keysym.scancode){
			case SDL_SCANCODE_SPACE:
				OutputDebugString("Space!");
				if (state == PLAY) { playerShoot(); }
				else if (state != QUIT) {
					resetGame();
					ans.newState = PLAY;
				}
				break;
			case SDL_SCANCODE_ESCAPE:
				ans = { QUIT, false };
				break;
			}
		}
	}

	return ans;
}

void GameClass::physics(){
	//Move dynamic objects
	for (size_t i = 0; i < dynamics.size(); i++){
		if (!dynamics[i].getVisibility()){ continue; }
		//Assume we are not in contact w/ anything
		dynamics[i].noTouch();

		dynamics[i].moveY(TIMESTEP, FRIC, 0);
		dynamics[i].moveX(TIMESTEP, FRIC);

		for (size_t j = 0; j < dynamics.size(); j++){
			if (i == j || !dynamics[j].getVisibility()) { continue; }
			if (dynamics[i].collide(dynamics[j])){
				Mix_PlayChannel(-1, hitRock, 0);
				dynamics[j].setVisibility(false);
				if (i > PLAYER && j > PLAYER) {
					asteroidCount -= 2;
					dynamics[i].setVisibility(false);
				}
				if (i == PLAYER || j == PLAYER){
					asteroidCount--;
					state = LOSE;
					stateText.changeText("You died!");
				}
				asteroidCountText.changeText("# left:" + std::to_string(asteroidCount));
			}
		}
		if (state == PLAY && !asteroidCount) {
			state = WIN;
			stateText.changeText("You won!");
		}
	}
	
	//Move beams
	for (size_t i = 0; i < beams.size(); i++){
		if (!beams[i].getVisibility()){ continue; }

		beams[i].moveByAngle(BEAMSPEED, TIMESTEP);

		//If out of screen, disappear
		float newX = beams[i].getX(); float newY = beams[i].getY();
		if (newX < player->getX() - UNIT_WIDTH || newX > player->getX() + UNIT_WIDTH ||
			newY < player->getY() - UNIT_HEIGHT || newY > player->getY() + UNIT_HEIGHT)
		{ beams[i].setVisibility(false); }

		//See if we hit an asteroid
		for (size_t j = PLAYER + 1; j < dynamics.size(); j++){
			if (dynamics[j].getVisibility() && beams[i].collide(dynamics[j])){
				beams[i].setVisibility(false);
				dynamics[j].setVisibility(false);
				Mix_PlayChannel(-1, hitRock, 0);
				asteroidCount--;
				asteroidCountText.changeText("# left:" + std::to_string(asteroidCount));
			}
		}
		if (!asteroidCount) {
			state = WIN;
			stateText.changeText("You won!");
		}
	}
}

bool GameClass::run(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastTickCount;
	lastTickCount = ticks;
	
	float fixedElapsed = elapsed + leftover;
	if (fixedElapsed > TIMESTEP * MAX_STEPS) { fixedElapsed = TIMESTEP * MAX_STEPS;	}
	while (state == PLAY && fixedElapsed >= TIMESTEP) {
		fixedElapsed -= TIMESTEP;
		physics();
	}
	leftover = fixedElapsed;

	StateAndRun sar = handleEvents();
	state = (GameState) sar.newState;

	if (state == PLAY) {
		pollForPlayer();
		renderGame();
	}
	else if (state != QUIT){
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		stateText.draw();
		SDL_GL_SwapWindow(displayWindow);
	}
	
	return sar.keepRunning;
}

void GameClass::drawStars(){
	glVertexPointer(2, GL_FLOAT, 0, stars.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glPointSize(3.0);

	glColorPointer(4, GL_FLOAT, 0, starColors.data());
	glEnableClientState(GL_COLOR_ARRAY);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_POINTS, 0, stars.size() / 2);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_BLEND);
}
void GameClass::drawAiming(){
	std::vector<float> aiming; std::vector<float> aimingColors;
	for (size_t i = 0; i < dynamics.size(); i++){
		if (!dynamics[i].getVisibility()) { continue; }
		//Show direction of distant asteroids!
		float px = dynamics[i].getX(); float py = dynamics[i].getY();
		if (px > player->getX() + UNIT_WIDTH){
			px = player->getX() + UNIT_WIDTH - 0.01f;
		}
		else if (px < player->getX() - UNIT_WIDTH){
			px = player->getX() - UNIT_WIDTH + 0.01f;
		}
		if (py > player->getY() + UNIT_HEIGHT){
			py = player->getY() + UNIT_HEIGHT - 0.01f;
		}
		else if (py < player->getY() - UNIT_HEIGHT){
			py = player->getY() - UNIT_HEIGHT + 0.01f;
		}
		if (px != dynamics[i].getX() || py != dynamics[i].getY()){
			aiming.push_back(px); aiming.push_back(py);
			aimingColors.push_back(1.0f);
			aimingColors.push_back(0.0f);
			aimingColors.push_back(0.0f);
		}
	}
	glVertexPointer(2, GL_FLOAT, 0, aiming.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glPointSize(5.0);
	glColorPointer(3, GL_FLOAT, 0, aimingColors.data());
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_POINTS, 0, aiming.size() / 2);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void GameClass::renderGame(){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//stars have PARALLAX!
	glTranslatef(-player->getX()*0.1f, -player->getY()*0.1f, 0);
	drawStars();
	glLoadIdentity();

	//Rest of objects normally shifted
	glTranslatef(-player->getX(), -player->getY(), 0);
	for (size_t i = 0; i < beams.size(); i++){ beams[i].draw(); }
	//Aiming = red dots on edge of screen
	drawAiming();
	for (size_t i = 0; i < dynamics.size(); i++){ dynamics[i].draw(); }
	
	glLoadIdentity();
	asteroidCountText.draw();

	SDL_GL_SwapWindow(displayWindow);
}