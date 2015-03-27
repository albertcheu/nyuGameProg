#include "GameClass.h"

GameClass::~GameClass(){
	for (size_t i = 0; i < beams.size(); i++) { beams[i].freeSound(); }
	//OutputDebugString("Freed beam sounds");
	Mix_FreeChunk(pickupSound);
	//OutputDebugString("Freed pickup sound");
	Mix_FreeMusic(music);
	//OutputDebugString("Freed ambient music");
	SDL_Quit();
	//OutputDebugString("Quit sdl");
}
TextureData GameClass::loadOpenGL(){
	//Boilerplate
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Totally not Metroid",
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

	return LoadTextureRGBA("mfTRO.png");
}
GameClass::GameClass()
	: lastTickCount(0), leftover(0), player(NULL), lookLeft(true), frameChange(0), elapsed(0),
	whichRed(0), whichYellow(NUMBEAMS), whichGreen(2*NUMBEAMS), whichBlue(3*NUMBEAMS),
	pool(loadOpenGL()){
	
	createDoorSprite(redDoor, 14.0f); createDoorSprite(yellowDoor, 12.0f);
	createDoorSprite(greenDoor, 10.0f); createDoorSprite(blueDoor, 8.0f);
	
	createPlayer(); createPickups(); createBeams();

	loadLevel("levelOne.txt", pool);

	music = Mix_LoadMUS("Underclocked_EricSkiff.mp3");
	Mix_PlayMusic(music, -1);
}

void GameClass::createDoorSprite(Sprite& d, float u_offset){
	d = Sprite(pool.id, TILEPIX * u_offset / pool.width, 0,
		1.0f * TILEPIX / pool.width, TILEPIX * 4.0f / pool.height);
}

void GameClass::createPlayer(){
	spriteSheet = LoadTextureRGBA("MetroidZeroMissionSheet1.png");
	int swidth = spriteSheet.width; int sheight = spriteSheet.height;

	//Standing
	cycles.push_back(AnimCycle(5, 0.5f, 559.0f / sheight, -1, 31.02f / swidth, 36.0f / sheight));
	cycles.push_back(AnimCycle(5, 0.5f, 559.0f / sheight, 1, 31.02f / swidth, 36.0f / sheight));
	
	//Running, left
	cycles.push_back(AnimCycle(6, 0.5f, 91.0f / sheight, -1, 37.5f / swidth, 36.0f / sheight));
	cycles.back().merge(AnimCycle(6, 0.5f, 131.0f / sheight, -1, 37.0f / swidth, 36.0f / sheight));
	cycles.back().setFrame(8, 81.0f / swidth, 35.0f / swidth);
	size_t arr[10] = { 0, 9, 1, 2, 7, 8, 4, 11, 5, 6 };
	cycles[RUNLEFT].reorder(10, arr);
	//right
	cycles.push_back(AnimCycle(6, 0.5f, 91.0f / sheight, 1, 37.5f / swidth, 36.0f / sheight));
	cycles.back().merge(AnimCycle(6, 0.5f, 131.0f / sheight, 1, 37.0f / swidth, 36.0f / sheight));
	cycles.back().setFrame(8, (swidth - 45.0f) / swidth, 35.0f / swidth);
	cycles[RUNRIGHT].reorder(10, arr);
	
	float playerHeight = 0.17f;
	Sprite p(spriteSheet.id, 217.0f/swidth, 3.0f/sheight, 16.0f / swidth, 38.0f / sheight);
	dynamics.push_back(Dynamic(0, 0, playerHeight, playerHeight, p));
	
	
}

void GameClass::createPickups(){
	pickupSound = Mix_LoadWAV("powerup.wav");
	for (size_t i = RED; i <= BLUE; i++){
		if (i > RED){ pickups.push_back(Pickup(pool, 10.0f + i - 1)); }
		else { pickups.push_back(Pickup()); }//placeholder for easy array access
	}
}

void GameClass::createBeams(){
	TextureData btd = LoadTextureRGB("beams.png");
	for (unsigned int i = RED; i <= BLUE; i++){
		//Color
		BeamColor bc = (BeamColor)(RED + i);
		//Sprite
		Sprite b(btd.id, 0, i*0.25f, 1.0f, 0.25f);
		//Sound file
		char buf[11]; sprintf_s(buf, 11, "beam%d.wav", (i + 1));
		Mix_Chunk* sound_ptr = Mix_LoadWAV(buf);
		//OutputDebugString(std::to_string((int)sound_ptr).c_str());

		//Actually make the beams now
		for (int j = 0; j < NUMBEAMS; j++){
			beams.push_back(Beam(0.03f, 0.01f, b, bc, sound_ptr));
		}
	}
}

void GameClass::loadLevel(const char* fname, TextureData texSource){
	theLevel = Level(fname, texSource, TILEPIX, TILECOUNTX, TILECOUNTY);

	const WhereToStart* wts;
	while (wts = theLevel.getNext()){
		if (wts->typeName == "PlayerStart") { dynamics[PLAYER].setPos(wts->x, wts->y); }
		else if (wts->typeName == "pickup") {
			if (wts->name == "yellow") { pickups[YELLOW].activate(wts->x,wts->y); }
			else if (wts->name == "green"){ pickups[GREEN].activate(wts->x, wts->y); }
			else if (wts->name == "blue") { pickups[BLUE].activate(wts->x, wts->y); }
		}
		else if (wts->typeName == "enemy"){
			//push_back enemies to the vector of dynamics
		}
		else{
			Sprite s = redDoor; BeamColor color = RED;
			if (wts->typeName == "yellow") { s = yellowDoor; color = YELLOW; }
			else if (wts->typeName == "green") { s = greenDoor; color = GREEN; }
			else if (wts->typeName == "blue") { s = blueDoor; color = BLUE; }
			float y = wts->y - TILEUNITS / 2;
			//left door goes right, right door goes left
			doors.push_back(Door(wts->x, y, s, color, BEAMDIR_RIGHT));
			doors.push_back(Door(wts->x+TILEUNITS*3, y, s, color, BEAMDIR_LEFT));
			doors.back().setAngle(180.0f);
		}
	}

	//a vector's location in memory changes; any pointers must be defined after pushes
	player = &dynamics[PLAYER];
	for (size_t i = 0; i < doors.size(); i += 2){
		doors[i].setComplement(&doors[i + 1]);
		doors[i + 1].setComplement(&doors[i]);
	}
}

void GameClass::pollForPlayer(){
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT]) {
		lookLeft = true;
		player->setAx(-MOVE);
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		lookLeft = false;
		player->setAx(MOVE);
	}
	else { player->setAx(0); }
}

void GameClass::playerShoot(size_t& which, size_t cap){
	beams[which].fire(player->getX(), player->getY()+0.034f,
		lookLeft ? BEAMDIR_LEFT : BEAMDIR_RIGHT);
	which++;
	if (which == cap) { which = cap-NUMBEAMS; }
}

StateAndRun GameClass::handleEvents(){
	StateAndRun ans = { 0, true };

	//Keyboard (and close-window) events
	SDL_Event event; bool fireQ = false;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			ans = { 0, false };
		}
		else if (event.type == SDL_KEYDOWN){
			switch (event.key.keysym.scancode){
			case SDL_SCANCODE_SPACE:
				if (player->getBottom()) { player->setVy(JUMP); }
				break;
			case SDL_SCANCODE_ESCAPE:
				ans = { 0, false };
				break;
			case SDL_SCANCODE_Q:
				if (!fireQ){
					fireQ = true;
					playerShoot(whichRed, NUMBEAMS);
				}
				break;
			case SDL_SCANCODE_W:
				if (pickups[YELLOW].have()){ playerShoot(whichYellow, 2 * NUMBEAMS); }
				break;
			case SDL_SCANCODE_E:
				if (pickups[GREEN].have()){ playerShoot(whichGreen, 3 * NUMBEAMS); }
				break;
			case SDL_SCANCODE_R:
				if (pickups[BLUE].have()){ playerShoot(whichBlue, 4 * NUMBEAMS); }
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

		//Useful info
		float hh = dynamics[i].getHalfHeight();
		float hw = dynamics[i].getHalfWidth(); float quart = hh / 2.0f;

		//Move in y and resolve collisions
		float newY = 0;
		dynamics[i].moveY(TIMESTEP, FRIC_Y, GRAVITY);
		float x = dynamics[i].getX(); float y = dynamics[i].getY();
		for (int j = -1; j <= 1; j++){//Test top-right, top-mid, top-left (same for bottom)
			newY = theLevel.tileCollide(x+0.9f*hw*j, y - hh, y, hh, true);
			if (newY != y){ dynamics[i].stickBottom(newY); }
			newY = theLevel.tileCollide(x+0.9f*hw*j, y + hh, y, hh, true);
			if (newY != y){ dynamics[i].stickTop(newY); }
		}		
		
		//Move in x and resolve collisions
		dynamics[i].moveX(TIMESTEP, FRIC_X);
		float newX = 0;
		for (int j = 1; j < 4; j++){//Test three points on each side
			newX = theLevel.tileCollide(x - hw, newY - hh + quart*j, x, hw, false);
			if (newX != x) { dynamics[i].stickLeft(newX); }
			newX = theLevel.tileCollide(x + hw, newY - hh + quart*j, x, hw, false);
			if (newX != x) { dynamics[i].stickRight(newX); }
		}

		//Can't walk thru doors either
		for (size_t j = 0; j < doors.size(); j++){
			if (!(doors[j].getVisibility() && doors[j].collide(dynamics[i]))) { continue; }
			//Assuming we cannot jump on a door, only collisions are horizontal
			newX = depenetrate(newX, hw, doors[j].getX(), doors[j].getHalfWidth());
			if (newX < dynamics[i].getX()) { dynamics[i].stickRight(newX); }
			else if (newX > dynamics[i].getX()) { dynamics[i].stickLeft(newX); }
		}
	}

	//Move beams (only go horizontally)
	for (size_t i = 0; i < beams.size(); i++){
		if (!beams[i].getVisibility()){ continue; }
		int dir = beams[i].getDir();
		float newX = beams[i].getX() + TIMESTEP*BEAMSPEED*dir;
		beams[i].setX(newX);

		if (newX != theLevel.tileCollide(newX,beams[i].getY(),newX,0,false)){
			beams[i].setVisibility(false);
		}
		for (size_t j = 0; j < doors.size(); j++){
			//Hit a door?
			if (beams[i].hit(doors[j])) { break; }
		}
	}
	
	//Move doors (also limited to x-axis)
	for (size_t i = 0; i < doors.size(); i++){
		if (!(doors[i].getVisibility() && doors[i].moving())){ continue; }
		doors[i].setX(doors[i].getX() + doors[i].getDir()*TIMESTEP*BEAMSPEED/4.0f);
		doors[i].disappear();
	}

	//Check if we got any of the pickups
	for (size_t i = 0; i < pickups.size(); i++){ pickups[i].hit(player, pickupSound); }
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
	animatePlayer();

	renderGame();
	
	return sar.keepRunning;
}

void GameClass::animatePlayer(){
	if (lastTickCount - frameChange >= 0.1f){
		SpriteFrame sf;
		if (fabs(player->getVx()) < 0.02f || player->getLeft() || player->getRight()) {
			if (lookLeft) { sf = cycles[STANDLEFT].getNext(); }
			else { sf = cycles[STANDRIGHT].getNext(); }
		}
		else{
			if (lookLeft) { sf = cycles[RUNLEFT].getNext(); }
			else { sf = cycles[RUNRIGHT].getNext(); }
		}
		frameChange = lastTickCount;
		player->setFrame(sf);
	}
}

void GameClass::renderGame(){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-player->getX(), -player->getY(), 0);

	for (size_t i = 0; i < beams.size(); i++){ beams[i].draw(); }

	for (size_t i = 0; i < dynamics.size(); i++){ dynamics[i].draw(); }
	
	for (size_t i = 0; i < doors.size(); i++){ doors[i].draw(); }
	
	for (size_t i = 0; i < pickups.size(); i++){ pickups[i].draw(); }

	theLevel.draw();

	SDL_GL_SwapWindow(displayWindow);
}