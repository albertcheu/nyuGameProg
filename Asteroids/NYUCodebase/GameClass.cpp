#include "GameClass.h"

GameClass::~GameClass(){
	Mix_FreeChunk(sound_ptr);
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
	: lastTickCount(0), leftover(0), player(NULL), frameChange(0), elapsed(0), which(0),
	spriteSheet(loadOpenGL()){
	
	swidth = spriteSheet.width; sheight = spriteSheet.height;

	createPlayer();
	createAsteroids();
	createBeams();
	player = &(dynamics[PLAYER]);
}

void GameClass::createPlayer(){
	float playerHeight = 0.17f; float ratio = 99.0f / 75;
	//<SubTexture name="playerShip1_blue.png" x="211" y="941" width="99" height="75"/>
	Sprite p(spriteSheet.id, 211.0f/swidth, 941.0f/sheight, 99.0f / swidth, 75.0f / sheight);
	dynamics.push_back(Dynamic(0, 0, playerHeight*ratio, playerHeight, p, NOT_ENEMY));

	
}

void GameClass::createAsteroids(){
	//< x = "406" y = "234" width = "28" height = "28" / >
	Sprite ast(spriteSheet.id, 406.0f / swidth, 234.0f / sheight, 28.0f / swidth, 28.0f / sheight);
	dynamics.push_back(Dynamic(0.5f, 0.5f, 0.15f, 0.15f, ast, NOT_ENEMY));

	//<SubTexture name="meteorBrown_med1.png" x="651" y="447" width="43" height="43"/>
	ast = Sprite(spriteSheet.id, 651.0f / swidth, 447.0f / sheight, 43.0f / swidth, 43.0f / sheight);
	dynamics.push_back(Dynamic(-0.5f, -0.5f, 0.2f, 0.2f, ast, NOT_ENEMY));
}

void GameClass::createBeams(){
	TextureData btd = LoadTextureRGB("beams.png");
	//Color
	BeamColor bc = RED;
	//Sprite
	Sprite b(btd.id, 0, 0, 1.0f, 0.25f);
	//Sound file
	sound_ptr = Mix_LoadWAV("beam1.wav");
	//Actually make the beams now
	for (int j = 0; j < NUMBEAMS; j++){
		beams.push_back(Beam(0.03f, 0.01f, b, bc, sound_ptr));
	}
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
	StateAndRun ans = { 0, true };

	//Keyboard (and close-window) events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			ans = { 0, false };
		}
		else if (event.type == SDL_KEYDOWN){
			switch (event.key.keysym.scancode){
			case SDL_SCANCODE_SPACE:
				playerShoot();
				break;
			case SDL_SCANCODE_ESCAPE:
				ans = { 0, false };
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
			dynamics[i].collide(dynamics[j]);
		}

	}
	
	//Move beams
	for (size_t i = 0; i < beams.size(); i++){
		if (!beams[i].getVisibility()){ continue; }
		float vx = BEAMSPEED * cos(beams[i].getAngle() * M_PI / 180.0f);
		float vy = BEAMSPEED * sin(beams[i].getAngle() * M_PI / 180.0f);

		float newX = beams[i].getX() + TIMESTEP*vx;

		float newY = beams[i].getY() + TIMESTEP*vy;

		beams[i].setPos(newX, newY);
		if (fabs(newX) > UNIT_WIDTH || fabs(newY) > UNIT_HEIGHT)
			{ beams[i].setVisibility(false); }
	}
	
}

bool GameClass::run(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastTickCount;
	lastTickCount = ticks;
	
	float fixedElapsed = elapsed + leftover;
	if (fixedElapsed > TIMESTEP * MAX_STEPS) { fixedElapsed = TIMESTEP * MAX_STEPS;	}
	while (fixedElapsed >= TIMESTEP) {
		fixedElapsed -= TIMESTEP;
		physics();
	}
	leftover = fixedElapsed;

	StateAndRun sar = handleEvents();

	pollForPlayer();

	renderGame();
	
	return sar.keepRunning;
}

void GameClass::renderGame(){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(-player->getX(), -player->getY(), 0);

	for (size_t i = 0; i < beams.size(); i++){ beams[i].draw(); }

	for (size_t i = 0; i < dynamics.size(); i++){ dynamics[i].draw(); }

	SDL_GL_SwapWindow(displayWindow);
}