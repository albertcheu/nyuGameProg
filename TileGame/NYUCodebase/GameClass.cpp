#include "GameClass.h"

GameClass::~GameClass(){ SDL_Quit(); }
GameClass::GameClass()
	: lastTickCount(0), leftover(0), player(NULL), lookLeft(true), frameChange(0), whichBeam(0)
	{
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
	
	pool = LoadTextureRGB("mfTRO.jpg");
	redDoor = Sprite(pool.id, TILEPIX * 14.0f / pool.width, 0,
		1.0f * TILEPIX / pool.width, TILEPIX * 4.0f / pool.height);
	yellowDoor = Sprite(pool.id, TILEPIX * 12.0f / pool.width, 0,
		1.0f * TILEPIX / pool.width, TILEPIX * 4.0f / pool.height);
	greenDoor = Sprite(pool.id, TILEPIX * 10.0f / pool.width, 0,
		1.0f * TILEPIX / pool.width, TILEPIX * 4.0f / pool.height);
	blueDoor = Sprite(pool.id, TILEPIX * 8.0f / pool.width, 0,
		1.0f * TILEPIX / pool.width, TILEPIX * 4.0f / pool.height);

	theLevel = Level("levelOne.txt", pool, TILEPIX, TILECOUNTX, TILECOUNTY);
	spriteSheet = LoadTextureRGBA("superPowerSuit.png");
	//OutputDebugString("Created level");
	fillEntities();
	//OutputDebugString("Made entities");
}

void GameClass::fillEntities(){
	int swidth = spriteSheet.width; int sheight = spriteSheet.height;
	cycles.push_back(AnimCycle(swidth, sheight, 25, 42, 3, 1));
	cycles.push_back(AnimCycle(swidth, sheight, 25, 42, 3, 1, 3));
	cycles.push_back(AnimCycle(swidth, sheight, 35, 41, 10, 3));
	cycles.push_back(AnimCycle(swidth, sheight, 35, 41, 10, 7));

	//play as Samus
	//26x46, 11th row
	float y = 64.0f * 11 - 46; float x = (64.0f - 26.0f) / 2;
	Sprite p(spriteSheet.id, x / swidth, y / sheight, 26.0f / swidth, 46.0f / sheight);
	float playerHeight = 0.17f;
	float playerWidth = playerHeight*26.0f/46.0f;
	dynamics.push_back(Dynamic(0, 0, playerWidth, playerHeight, p));

	player = &dynamics[PLAYER];//do last! the vector's location in memory changes after pushes
	
	const WhereToStart* wts;
	while (wts = theLevel.getNext()){
		if (wts->typeName == "PlayerStart") { dynamics[PLAYER].setPos(wts->x, wts->y); }
		//Pickups
		//Enemies
		else{
			Sprite s = redDoor; BeamColor color = RED;
			if (wts->typeName == "yellow") { s = yellowDoor; color = YELLOW; }
			else if (wts->typeName == "green") { s = greenDoor; color = GREEN; }
			else if (wts->typeName == "blue") { s = blueDoor; color = BLUE; }
			y = wts->y - TILEUNITS / 2;
			doors.push_back(Door(wts->x, y, s, color, 1));
			doors.push_back(Door(wts->x+TILEUNITS*3, y, s, color, -1));
			doors.back().setAngle(180.0f);
		}
	}
	for (size_t i = 0; i < doors.size(); i += 2){
		doors[i].setComplement(&doors[i + 1]);
		doors[i + 1].setComplement(&doors[i]);
	}

	TextureData btd = LoadTextureRGB("beams.png");
	Sprite b(btd.id, 0, 0, 1.0f, 0.27f);
	for (int i = 0; i < 10; i++){ beams.push_back(Beam(0.03f, 0.01f, b, RED)); }
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
	case SDL_SCANCODE_SPACE:
		if (player->getBottom()) { player->setVy(JUMP); }
		break;
	case SDL_SCANCODE_ESCAPE:
		ans = { 0, false };
		break;
	case CLOSE_WINDOW:
		ans = { 0, false };
		break;
	case SDL_SCANCODE_Q:
		//OutputDebugString("Pressed q");
		beams[whichBeam].fire(player->getX(), player->getY(), lookLeft?180.0f:0);
		whichBeam++;
		if (whichBeam == beams.size()) { whichBeam = 0; }
		break;
	case SDL_SCANCODE_W:
		OutputDebugString("Pressed w");
		break;
	case SDL_SCANCODE_E:
		OutputDebugString("Pressed e");
		break;
	case OTHER: break;
	}

	movePlayer(elapsed);

	return ans;
}

void GameClass::physics(){
	//Move dynamic objects
	for (size_t i = 0; i < dynamics.size(); i++){
		dynamics[i].noTouch();
		
		//Move in y and resolve collisions
		dynamics[i].moveY(TIMESTEP, FRIC_Y, GRAVITY);
		
		float x = dynamics[i].getX(); float y = dynamics[i].getY();
		float hh = dynamics[i].getHalfHeight();
		float newY = theLevel.tileCollide(x, y - hh, y, hh, true);
		if (newY != y){ dynamics[i].stickBottom(newY); }
		newY = theLevel.tileCollide(x, y + hh, y, hh, true);
		if (newY != y){ dynamics[i].stickTop(newY); }
		
		//Move in x and resolve collisions
		dynamics[i].moveX(TIMESTEP, FRIC_X);
		float hw = dynamics[i].getHalfWidth(); float quart = hh / 2.0f;
		float newX = 0;
		for (int j = 1; j < 4; j++){
			newX = theLevel.tileCollide(x - hw, newY - hh + quart*j, x, hw, false);
			if (newX != x) { dynamics[i].stickLeft(newX); }
			newX = theLevel.tileCollide(x + hw, newY - hh + quart*j, x, hw, false);
			if (newX != x) { dynamics[i].stickRight(newX); }
		}

		//Can't walk thru doors either
		for (size_t j = 0; j < doors.size(); j++){
			if (!(doors[j].getVisibility() && doors[j].collide(dynamics[i]))) { continue; }
			//Assume we cannot jump on a door; only collisions are horizontal
			newX = depenetrate(newX, hw, doors[j].getX(), doors[j].getHalfWidth());
			if (newX < dynamics[i].getX()) { dynamics[i].stickRight(newX); }
			else if (newX > dynamics[i].getX()) { dynamics[i].stickLeft(newX); }
			
		}

	}
	//Move beams
	for (size_t i = 0; i < beams.size(); i++){
		if (!beams[i].getVisibility()){ continue; }
		int dir = beams[i].getAngle() == 180.0f ? -1 : 1;
		float newX = beams[i].getX() + TIMESTEP*BEAMSPEED*dir;
		beams[i].setX(newX);

		if (newX != theLevel.tileCollide(newX,beams[i].getY(),newX,0,false)){
			beams[i].setVisibility(false);
		}
		for (size_t j = 0; j < doors.size(); j++){
			//Hit a door?
			if (beams[i].collide(doors[j])){
				doors[j].hit(beams[i].getColor());
				beams[i].setVisibility(false);
			}
		}
	}
	//Move doors
	for (size_t i = 0; i < doors.size(); i++){
		if (!(doors[i].getVisibility() && doors[i].moving())){ continue; }
		doors[i].setX(doors[i].getX() + doors[i].getDir()*TIMESTEP*BEAMSPEED/4.0f);
		doors[i].disappear();
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
	//OutputDebugString("Physicked");

	StateAndRun sar = updateGame(elapsed);
	//OutputDebugString("Animated");
	renderGame();
	
	return sar.keepRunning;
}

void GameClass::renderGame(){
	glClear(GL_COLOR_BUFFER_BIT);

	for (size_t i = 0; i < dynamics.size(); i++){
		dynamics[i].draw(-player->getX(), -player->getY());
	}
	for (size_t i = 0; i < beams.size(); i++){
		beams[i].draw(-player->getX(), -player->getY());
	}
	for (size_t i = 0; i < doors.size(); i++){
		doors[i].draw(-player->getX(), -player->getY());
	}
	theLevel.draw(-player->getX(), -player->getY());

	SDL_GL_SwapWindow(displayWindow);
}