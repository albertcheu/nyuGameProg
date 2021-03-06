#include "GameClass.h"

Samus::Samus():Dynamic(){}
Samus::Samus(float x, float y, float width, float height, Sprite s, int swidth, int sheight)
	: Dynamic(x, y, width, height, s, SAMUS), cycles(cycles),
	lookLeft(false), standing(true), aimUp(false){
	//Standing
	cycles.push_back(AnimCycle(5, 0.5f, 559.0f / sheight, -1, 31.02f / swidth, 36.0f / sheight));
	cycles.push_back(AnimCycle(5, 0.5f, 559.0f / sheight, 1, 31.02f / swidth, 36.0f / sheight));

	//Running
	size_t arr[10] = { 0, 9, 1, 2, 7, 8, 4, 11, 5, 6 };
	//left	
	cycles.push_back(AnimCycle(6, 0.5f, 91.0f / sheight, -1, 37.5f / swidth, 36.0f / sheight));
	cycles[RUNLEFT].merge(AnimCycle(6, 0.5f, 131.0f / sheight, -1, 37.0f / swidth, 36.0f / sheight));
	cycles[RUNLEFT].setFrame(8, 81.0f / swidth, 35.0f / swidth);
	//cycles[RUNLEFT].setFrame(1, 92.0f / swidth, 35.0f / swidth);
	//cycles[RUNLEFT].setFrame(7, 81.0f / swidth, 35.0f / swidth);
	cycles[RUNLEFT].setFrame(9, 113.0f / swidth, 35.0f / swidth);

	cycles[RUNLEFT].reorder(10, arr);
	//right
	cycles.push_back(AnimCycle(6, 0.5f, 91.0f / sheight, 1, 37.0f / swidth, 36.0f / sheight));
	cycles[RUNRIGHT].merge(AnimCycle(6, 0.5f, 131.0f / sheight, 1, 37.0f / swidth, 36.0f / sheight));
	cycles[RUNRIGHT].setFrame(8, (swidth - 118.0f) / swidth, 35.0f / swidth);
	cycles[RUNRIGHT].setFrame(1, (swidth - 76.0f) / swidth, 35.0f / swidth);
	cycles[RUNRIGHT].setFrame(7, (swidth - 80.0f) / swidth, 35.0f / swidth);
	cycles[RUNRIGHT].setFrame(9, (swidth - 154.0f) / swidth, 35.0f / swidth);//154

	cycles[RUNRIGHT].reorder(10, arr);

	//Sitting
	cycles.push_back(AnimCycle(1, 0.5f, 597.0f / sheight, -1, 28.2f / swidth, 27.0f / sheight));
	cycles.push_back(AnimCycle(1, 0.5f, 597.0f / sheight, 1, 28.2f / swidth, 27.0f / sheight));
	
	//STANDLEFTUP//STANDRIGHTUP
	cycles.push_back(AnimCycle(1, 0.5f, 632.0f / sheight, -1, 24.02f / swidth, 45.0f / sheight));
	cycles.push_back(AnimCycle(1, 0.5f, 632.0f / sheight, 1, 24.02f / swidth, 45.0f / sheight));
	
	//INAIRLEFT, INAIRRIGHT
	cycles.push_back(AnimCycle(1, 178.0f / swidth, 414.0f / sheight, -1, 35.0f / swidth, 35.0f / sheight));
	cycles.push_back(AnimCycle(1, 272.0f / swidth, 414.0f / sheight, 1, 35.0f / swidth, 35.0f / sheight));
}
void Samus::standUp(){
	standing = true;
	setSize(0.17f, 0.17f);
}
void Samus::sitDown(){
	standing = false;
	setSize(0.16f, 0.13f);
}
void Samus::nextFrame(){
	SpriteFrame sf;
	
	//If in air
	if (!getBottom()){
		if (lookLeft) { sf = cycles[INAIRLEFT].getNext(); }
		else { sf = cycles[INAIRRIGHT].getNext(); }
	}

	//Stationary (or walking into a wall)
	else if (ax == 0 || getLeft() || getRight()) {
		if (standing){
			if (lookLeft) {
				if (aimUp) { sf = cycles[STANDLEFTUP].getNext(); }
				else { sf = cycles[STANDLEFT].getNext(); }
			}
			else {
				if (aimUp) { sf = cycles[STANDRIGHTUP].getNext(); }
				else { sf = cycles[STANDRIGHT].getNext(); }
			}
		}
		else{
			if (lookLeft) { sf = cycles[SITLEFT].getNext(); }
			else { sf = cycles[SITRIGHT].getNext(); }
		}
	}

	//Mobile
	else{
		if (lookLeft) { sf = cycles[RUNLEFT].getNext(); }
		else { sf = cycles[RUNRIGHT].getNext(); }
	}
	setFrame(sf);
}
bool Samus::collideBounce(Dynamic& enemy, float bounceMag){
	if (!enemy.getVisibility()){ return false; }
	//Check how much we need to depenetrate
	Vector v = pushOut(enemy);
	if (v.x == 0 && v.y == 0) { return false; }

	//Enemy is to our right and nothing to our left
	if (x <= enemy.getX() && !touchLeft) { vx = -bounceMag; }
	//Enemy is to our left and nothing to our right
	else if (enemy.getX() < x && !touchRight){ vx = bounceMag; }

	//Enemy above and nothing below
	if (y <= enemy.getY() && !touchBottom) { vy = -bounceMag; }
	//Enemy below and nothing above
	else if (enemy.getY() < y && !touchTop) { vy = bounceMag; }

	return true;
}

GameClass::~GameClass(){
	for (size_t i = 0; i < beams.size(); i+=NUMBEAMS) { beams[i].freeSound(); }
	Mix_FreeChunk(pickupSound);
	Mix_FreeChunk(hurtSound);
	Mix_FreeChunk(hitHopper);
	Mix_FreeChunk(hitRunner);
	Mix_FreeChunk(hitDoor);
	Mix_FreeChunk(hitShield);
	Mix_FreeChunk(move);
	Mix_FreeChunk(bump);
	Mix_FreeChunk(switchSound);

	Mix_FreeMusic(music);
	
	delete player;

	SDL_Quit();
}
TextureData GameClass::loadOpenGL(){
	//Boilerplate
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("MODERIT",
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

	return LoadTextureRGBA("sprites/mfTRO.png");
}
GameClass::GameClass()
	: lastTickCount(0), leftover(0), player(NULL), frameChange(0),
	elapsed(0), whichRed(0), whichYellow(NUMBEAMS), whichGreen(2 * NUMBEAMS),
	whichBlue(3 * NUMBEAMS), hurtTime(0), bossBeam(NULL), bossTime(0), state(MENU),
	pool(loadOpenGL()){
	
	hitDoor = Mix_LoadWAV("sounds/hitDoor.wav");
	hitShield = Mix_LoadWAV("sounds/hitShield.wav");
	createDoorSprite(redDoor, 14.0f); createDoorSprite(yellowDoor, 12.0f);
	createDoorSprite(greenDoor, 10.0f); createDoorSprite(blueDoor, 8.0f);
	
	createPlayer(); createPickups(); createBeams(); createEnemySprites();

	music = Mix_LoadMUS("sounds/Underclocked_EricSkiff.mp3");

	m = Menu(displayWindow,
		switchSound = Mix_LoadWAV("sounds/switch.wav"),
		move = Mix_LoadWAV("sounds/move.wav"),
		bump = Mix_LoadWAV("sounds/bump.wav")
		);
}

void GameClass::createDoorSprite(Sprite& d, float u_offset){
	d = Sprite(pool.id, TILEPIX * u_offset / pool.width, 0,
		1.0f * TILEPIX / pool.width, TILEPIX * 4.0f / pool.height);
}

void GameClass::createPlayer(){
	TextureData td = LoadTextureRGBA("sprites/MetroidZeroMissionSheet1.png");
	int swidth = td.width; int sheight = td.height;
	Sprite s(td.id, 217.0f/swidth, 3.0f/sheight, 16.0f / swidth, 38.0f / sheight);
	player = new Samus(0, 0, 0.17f, 0.17f, s, swidth, sheight);

	hurtSound = Mix_LoadWAV("sounds/hurt.wav");
	s = Sprite(LoadTextureRGBA("sprites/hitCircle.png").id, 0, 0, 1, 1);
	hurtFlash = Entity(0, 0, 2.66f, 2.0f, s);

	td = LoadTextureRGBA("sprites/font.png");
	healthDisplay = Text(td.id, 1.0f / 16.0f, 0.08f,
		0, 0.9f, std::to_string(player->changeHealth(0)));
	maxHealthDisplay = Text(td.id, 1.0f / 16.0f, 0.08f,
		0, 0.8f, std::to_string(player->changeMaxHealth(0)));
}

void GameClass::createPickups(){
	pickupSound = Mix_LoadWAV("sounds/powerup.wav");
	for (size_t i = RED; i <= BLUE; i++){
		if (i > RED){ pickups.push_back(Pickup(pool, 10.0f + i - 1)); }
		else { pickups.push_back(Pickup()); }//placeholder for easy array access
	}
	//There will always be at most 4 energy tanks
	for (int i = 0; i < NUMTANKS; i++){ pickups.push_back(Pickup(pool,8.0f)); }
}

void GameClass::createBeams(){
	TextureData btd = LoadTextureRGB("sprites/beams.png");
	TextureData ptd = LoadTextureRGBA("sprites/particle.png");
	for (unsigned int i = RED; i <= BLUE; i++){
		//Color
		BeamColor bc = (BeamColor)(RED + i);
		//Sprite
		Sprite b(btd.id, 0, i*0.25f, 1.0f, 0.25f);
		//Sound file
		char buf[20]; sprintf_s(buf, 20, "sounds/beam%d.wav", (i + 1));
		Mix_Chunk* sound_ptr = Mix_LoadWAV(buf);
		//Actually make the beams now
		for (int j = 0; j < NUMBEAMS; j++){
			beams.push_back(Beam(0.03f, 0.01f, b, bc, sound_ptr));
		}
		for (int j = 0; j < NUMBEAMS * NUMPARTICLES; j++){
			particles.push_back(Dynamic(0, 0, 0.02f, 0.02f,
				Sprite(ptd.id, 0.25f*i, 0, 0.25f, 1), SAMUS)
				);
			particles.back().setVisibility(false);
		}
	}
}

void GameClass::createEnemySprites(){
	TextureData etd = LoadTextureRGBA("sprites/hopper.png");
	hopperSprite = Sprite(etd.id, 0, 128.0f / 293.0f, 43.0f / 352.0f, 21.0f / 293.0f);
	hitHopper = Mix_LoadWAV("sounds/hitHopper.wav");

	etd = LoadTextureRGBA("sprites/runner.png");
	runnerSprite = Sprite(etd.id, 2.0f/84.0f, 2.0f/232.0f, 15.0f/84.0f, 15.0f/232.0f);
	hitRunner = Mix_LoadWAV("sounds/hitRunner.wav");

	etd = LoadTextureRGBA("sprites/flier.png");
	flierSprite = Sprite(etd.id, 0, 74.0f / 118.0f, 34.0f / 140.0f, 27.0f / 118.0f);

	etd = LoadTextureRGBA("sprites/boss.png");
	bossSprite = Sprite(etd.id, 0, 1000.0f/1282.0f, 80.0f / 640.0f, 80.0f/1282.0f);

	etd = LoadTextureRGBA("sprites/bossBeam.png");
	bbSprite = Sprite(etd.id, 0, 0, 1, 1);

	etd = LoadTextureRGBA("sprites/shield.png");
	shield = Entity(0,0,0.2f,0.2f,Sprite(etd.id, 0, 0, 0.33f, 0.25f));
	shieldHealth = SHIELD_HEALTH;	shieldRating = RED;
}

void GameClass::loadLevel(const char* fname, TextureData texSource){
	theLevel = Level(fname, texSource, TILEPIX, TILECOUNTX, TILECOUNTY);
	const WhereToStart* wts;
	while (wts = theLevel.getNext()){

		if (wts->typeName == "PlayerStart") { player->setPos(wts->x, wts->y); }
		//upgrades
		else if (wts->typeName == "pickup") {
			if (wts->name == "yellow") { pickups[YELLOW].activate(wts->x,wts->y); }
			else if (wts->name == "green"){ pickups[GREEN].activate(wts->x, wts->y); }
			else if (wts->name == "blue") { pickups[BLUE].activate(wts->x, wts->y); }
			else{//energy tank
				for (size_t i = BLUE + 1; i < pickups.size(); i++){
					if (!pickups[i].getVisibility()){
						pickups[i].activate(wts->x, wts->y);
						break;
					}
				}
			}
		}
		//push_back enemies to the vector of enemies
		else if (wts->typeName == "hopper"){
			enemies.push_back(Dynamic(wts->x, wts->y, 0.18f, 0.09f, hopperSprite, HOPPER));
		}
		else if (wts->typeName == "runner"){
			enemies.push_back(Dynamic(wts->x, wts->y, 0.07f, 0.082f, runnerSprite, RUNNER));
		}
		else if (wts->typeName == "flier"){
			enemies.push_back(Dynamic(wts->x, wts->y, 0.12f, 0.11f, flierSprite, FLIER));
			enemies.back().setAy(-GRAVITY);
		}
		else if (wts->typeName == "boss"){
			enemies.push_back(Dynamic(wts->x, wts->y, 0.17f, 0.17f, bossSprite, BOSS));
			enemies.back().setAy(-GRAVITY);
		}
		else{//Doors
			Sprite s = redDoor; BeamColor color = RED;
			if (wts->typeName == "yellow") { s = yellowDoor; color = YELLOW; }
			else if (wts->typeName == "green") { s = greenDoor; color = GREEN; }
			else if (wts->typeName == "blue") { s = blueDoor; color = BLUE; }
			float y = wts->y - TILEUNITS / 2;
			//left door goes right, right door goes left
			doors.push_back(Door(wts->x, y, s, color, BEAMDIR_RIGHT));
			doors.push_back(Door(wts->x+TILEUNITS*3, y, s, color, BEAMDIR_LEFT));
			doors.back().setAngle(180.0f); doors.back().buildTransform();
		}
	}

	for (size_t i = 0; i < doors.size(); i += 2){
		doors[i].setComplement(&doors[i + 1]);
		doors[i + 1].setComplement(&doors[i]);
	}

	enemies.push_back(Dynamic(0, 0, 0.09f, 0.09f, bbSprite, BOSS_BEAM));
	enemies.back().setVisibility(false);
	enemies.back().setAngle(270.0f);
	bossBeam = &(enemies.back());
}

void GameClass::pollForPlayer(){
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	player->aimUp = false;
	if (keys[SDL_SCANCODE_LEFT]) {
		player->standUp();
		player->lookLeft = true;
		player->setAx(-MOVE);
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		player->standUp();
		player->lookLeft = false;
		player->setAx(MOVE);
	}
	else {
		player->setAx(0);
		if (keys[SDL_SCANCODE_UP]){
			player->aimUp = true;
			player->setSize(0.13f, 0.2f);
		}
		else{
			player->aimUp = false;
			player->setSize(0.17f, (player->standing ? 0.17f : 0.13f));
		}
	}

	
}

void GameClass::playerShoot(size_t& which, size_t cap){
	float bx, by, theta;
	if (player->aimUp){
		bx = player->getX();
		by = player->getY() + player->getHalfHeight();
		theta = 90.0f;
	}
	else{
		bx = player->getX() + (player->lookLeft ? -1 : 1)*player->getHalfWidth();
		by = player->getY() + (player->standing ? 0.034f : 0.011f);
		theta = (player->lookLeft ? 180.0f : 0);
	}	

	beams[which].fire(bx, by, theta);

	which++;
	if (which == cap) { which = cap-NUMBEAMS; }
}

GameState GameClass::handleEvents(){
	GameState ans = PLAY;

	//Keyboard (and close-window) events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			ans = EXIT;
		}
		else if (event.type == SDL_KEYDOWN){
			switch (event.key.keysym.scancode){
			case SDL_SCANCODE_SPACE:
				if (player->getBottom()) {
					player->standUp();
					player->setVy(JUMP);
				}
				break;
			case SDL_SCANCODE_ESCAPE:
				Mix_HaltMusic();
				ans = MENU;
				break;
			case SDL_SCANCODE_Q:
				playerShoot(whichRed, NUMBEAMS);
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
			case SDL_SCANCODE_DOWN:
				player->sitDown();
				break;
			case SDL_SCANCODE_UP:
				player->standUp();
				break;
			}
		}
	}

	return ans;
}

bool GameClass::castToPlayer(Dynamic& d){
	//increment in lengths of TILEUNITS
	Vector v = Vector(player->getX() - d.getX(), player->getY() - d.getY(), 0);
	v.normalize();
	v = v * TILEUNITS;
	float x = d.getX(); float y = d.getY();
	while (!theLevel.solidTile(x, y)){
		x += v.x; y += v.y;
		if (player->contains(x, y)){ return true; }
	}
	return false;
}
void GameClass::moveDynamicY(Dynamic& d){
	float hh = d.getHalfHeight();	float hw = d.getHalfWidth();
	d.moveY(TIMESTEP, FRIC_Y, GRAVITY);
	float x = d.getX(); float y = d.getY();
	for (int j = -1; j <= 1; j++){//Test top-right, top-mid, top-left (same for bottom)
		float newY = theLevel.tileCollide(x + 0.9f*hw*j, y - hh, y, hh, true);
		if (newY != y){ d.stickBottom(newY); }
		newY = theLevel.tileCollide(x + 0.9f*hw*j, y + hh, y, hh, true);
		if (newY != y){ d.stickTop(newY); }
	}
}
void GameClass::moveDynamicX(Dynamic& d){
	float hh = d.getHalfHeight();	float hw = d.getHalfWidth();
	d.moveX(TIMESTEP, FRIC_X);
	float x = d.getX(); float y = d.getY(); float quart = hh / 2.0f;
	for (int j = 1; j < 4; j++){//Test three points on each side
		float newX = theLevel.tileCollide(x - hw, y - hh + quart*j, x, hw, false);
		if (newX != x) { d.stickLeft(newX); }
		newX = theLevel.tileCollide(x + hw, y - hh + quart*j, x, hw, false);
		if (newX != x) { d.stickRight(newX); }
	}
}
void GameClass::moveDynamic(Dynamic& d){
	//Assume we are not in contact w/ anything
	d.noTouch();
	//Move in y and resolve collisions with level
	moveDynamicY(d);
	//Move in x and resolve collisions with level
	moveDynamicX(d);

	//Can't walk thru doors either
	float hw = d.getHalfWidth();
	float x = d.getX();
	for (size_t j = 0; j < doors.size(); j++){
		if (doors[j].getVisibility()){ d.collide(doors[j]); }		
	}
}
void GameClass::moveHoriz(Dynamic& d){
	
	if (d.getLeft()) { d.setAx(ENEMY_MOVE); }
	else if (d.getRight())	{ d.setAx(-ENEMY_MOVE); }

	else if (d.getBottom()){
		if (!theLevel.solidTile(d.getX() - d.getHalfWidth() - 0.001f,
			d.getY() - d.getHalfHeight() - 0.001f)){ d.setAx(ENEMY_MOVE);	}
		if (!theLevel.solidTile(d.getX() + d.getHalfWidth() + 0.001f,
			d.getY() - d.getHalfHeight() - 0.001f)){ d.setAx(-ENEMY_MOVE); }
	}

	else if (d.getVx() == 0) { d.setAx(ENEMY_MOVE); }
	else { d.setAx(d.getVx() > 0 ? ENEMY_MOVE : -ENEMY_MOVE); }
	
}
void GameClass::moveEnemy(Dynamic& d){
	switch (d.getType()){
	case HOPPER:
		//Touching the ground (or player): hop
		if (d.getBottom()) { d.setVy(ENEMY_JUMP); }
		//Stalk player
		if (fabs(d.getX() - player->getX()) < 0.3f &&
			fabs(d.getY() - player->getY()) < 0.05f){		
			if (d.getX() < player->getX()){ d.setAx(ENEMY_MOVE); }
			else { d.setAx(-ENEMY_MOVE); }
		}
		break;
	case RUNNER:
		moveHoriz(d);
		break;
	case FLIER:
		moveHoriz(d);
		//Upon, touching the ground (or player), fly back up
		if (d.getBottom()) {
			d.setAy(-GRAVITY);
			d.setVy(ENEMY_JUMP);
		}
		//If player is visible beneath us, drop
		else if (fabs(player->getX() - d.getX()) < 0.1f &&
				d.getY() > player->getY() && castToPlayer(d)){
			d.setAy(0);
		}
		break;
	case BOSS:
		if (castToPlayer(d) && fabs(d.getX()-player->getX())<0.4f) {
			//fire at player if we're "fully charged"
			if (!bossBeam->getVisibility() && fabs(d.getX() - player->getX()) < 0.1f
				&& lastTickCount - bossTime > 1.4f){
				bossBeam->setPos(d.getX(), d.getY() - d.getHalfWidth());
				bossBeam->setVisibility(true);
				bossTime = lastTickCount;
			}
			//stalk player
			if (d.getX() < player->getX()){ d.setAx(ENEMY_MOVE); }
			else { d.setAx(-ENEMY_MOVE); }
		}
		else { moveHoriz(d); }
		break;
	}
}
void GameClass::weakenShield(){
	shieldHealth--;
	if (shieldHealth == 0 && shieldRating == BLUE) { shield.setVisibility(false); }
	else if (shieldHealth == 0) {
		shieldRating = (BeamColor)(shieldRating + 1);
		shieldHealth = SHIELD_HEALTH;
	}
	float u = (shieldHealth > 2 * SHIELD_HEALTH / 3) ? 0 :
		((shieldHealth > SHIELD_HEALTH / 3) ? 0.33f : 0.66f);
	shield.setUV(u, shieldRating*0.25f);
	Mix_PlayChannel(-1, hitShield, 0);
}

void GameClass::spawnParticles(size_t which, float x, float y){
	size_t start = which * NUMPARTICLES;
	for (size_t i = 0; i < NUMPARTICLES; i++){
		size_t whichParticle = start + i;
		particles[whichParticle].setPos(x, y);
		particles[whichParticle].setVisibility(true);
		//40-80%
		float frac = 0.01f * (40 + (1 + (rand() % 40)));
		particles[whichParticle].setVx(frac*BEAMSPEED*(i%2?-1:1));
		particles[whichParticle].setVy(frac*BEAMSPEED*(i*2<NUMPARTICLES?-1:1));
	}
}

void GameClass::physics(){
	moveDynamic(*player);

	for (size_t i = 0; i < enemies.size(); i++){
		if (!enemies[i].getVisibility() || !inScreen(enemies[i])){ continue; }
		moveDynamic(enemies[i]);
		moveEnemy(enemies[i]);
		if (player->collideBounce(enemies[i], HITSPEED)) {
			int change = DAMAGE_AMT[enemies[i].getType()];
			if (player->changeHealth(change) <= 0) { state = LOSE; }
			healthDisplay.changeText(std::to_string(player->changeHealth(0)));
			Mix_PlayChannel(-1, hurtSound, 0);
			hurtTime = lastTickCount;
			player->standUp();
			if (enemies[i].getType() == BOSS_BEAM){ enemies[i].reset(); }
		}
		if (enemies[i].getType() == BOSS_BEAM && enemies[i].getBottom()){
			enemies[i].reset();
		}
	}

	//Move beams
	for (size_t i = 0; i < beams.size(); i++){
		if (!beams[i].getVisibility()){ continue; }
		bool beamHitSomething = false;
		float radAngle = beams[i].getAngle() * M_PI / 180.0f;
		float newX = beams[i].getX() + (TIMESTEP*BEAMSPEED)*cos(radAngle);
		float newY = beams[i].getY() + (TIMESTEP*BEAMSPEED)*sin(radAngle);
		beams[i].setPos(newX, newY);

		if (newX != theLevel.tileCollide(newX,beams[i].getY(),newX,0,false)){
			beams[i].setVisibility(false);
			beamHitSomething = true;
		}
		if (newY != theLevel.tileCollide(newX, beams[i].getY(), newY, 0, true)){
			beams[i].setVisibility(false);
			beamHitSomething = true;
		}
		
		//Hit a door?
		for (size_t j = 0; j < doors.size(); j++){
			if (beams[i].hit(doors[j], hitDoor)) { beamHitSomething = true; break; }
		}

		//Hit boss' shield?
		if (shield.getVisibility() && beams[i].collide(shield)){
			beams[i].setVisibility(false);
			if (beams[i].getColor() != shieldRating) { continue; }
			weakenShield();
			continue;
		}

		//Hit an enemy?
		for (size_t j = 0; j < enemies.size(); j++){
			if (beams[i].hit(enemies[j])) {
				beamHitSomething = true;
				Mix_PlayChannel(-1, (enemies[j].getType() == RUNNER) ? hitRunner : hitHopper, 0);
				if (enemies[j].getType() == BOSS && enemies[j].changeHealth(0) <= 0){
					state = WIN;
				}
				break;
			}
		}

		if (beamHitSomething){ spawnParticles(i, beams[i].getX(), beams[i].getY()); }
	}
	
	for (size_t i = 0; i < particles.size(); i++){
		if (!particles[i].getVisibility()){ continue; }
		moveDynamic(particles[i]);
		if (particles[i].getTop() || particles[i].getBottom()){ particles[i].reset(); }
	}

	//Move doors (limited to x-axis)
	for (size_t i = 0; i < doors.size(); i++){
		if (!(doors[i].getVisibility() && doors[i].moving())){ continue; }
		doors[i].setX(doors[i].getX() + doors[i].getDir()*TIMESTEP*BEAMSPEED/4.0f);
		doors[i].disappear();
	}

	//Check if we got any of the pickups
	for (size_t i = 0; i < pickups.size(); i++){
		if (pickups[i].hit(player, pickupSound) && i > BLUE){
			int newMaxHealth = player->changeMaxHealth(100);
			maxHealthDisplay.changeText(std::to_string(newMaxHealth));
			int delta = newMaxHealth - player->changeHealth(0);
			healthDisplay.changeText(std::to_string(player->changeHealth(delta)));
		}
	}
}

bool GameClass::inScreen(Entity& subject){
	float leftEdge = player->getX() - UNIT_WIDTH;
	float rightEdge = player->getX() + UNIT_WIDTH;
	float topEdge = player->getY() - UNIT_HEIGHT;
	float bottomEdge = player->getY() + UNIT_HEIGHT;
	float x = subject.getX();
	float y = subject.getY();
	//not in screen = coordinate not in screen AND corners not in screen
	if (x > leftEdge && x < rightEdge && y > topEdge && y < bottomEdge){ return true; }

	//corners
	float hw = subject.getHalfWidth();
	float hh = subject.getHalfHeight();
	if (x+hw > leftEdge && x+hw < rightEdge && y-hh > topEdge && y-hh < bottomEdge){ return true; }
	if (x+hw > leftEdge && x+hw < rightEdge && y+hh > topEdge && y+hh < bottomEdge){ return true; }
	if (x - hw > leftEdge && x - hw < rightEdge && y - hh > topEdge && y - hh < bottomEdge){ return true; }
	if (x - hw > leftEdge && x - hw < rightEdge && y + hh > topEdge && y + hh < bottomEdge){ return true; }
	return false;
}

bool GameClass::run(){
	float ticks, fixedElapsed;
	GameState oldState = state;
	switch (state){
	case PLAY:
		ticks = (float)SDL_GetTicks() / 1000.0f;
		elapsed = ticks - lastTickCount;
		lastTickCount = ticks;

		fixedElapsed = elapsed + leftover;
		if (fixedElapsed > TIMESTEP * MAX_STEPS) { fixedElapsed = TIMESTEP * MAX_STEPS; }
		while (state == PLAY && fixedElapsed >= TIMESTEP) {
			fixedElapsed -= TIMESTEP;
			physics();
		}
		leftover = fixedElapsed;

		if (state == PLAY){
			state = handleEvents();
			pollForPlayer();
			animate();
		}

		renderGame();
		break;
	case MENU:
		state = m.handleEvents();
		m.renderMenu(MENU);
		break;
	case LOSE:
		Mix_HaltMusic();
		state = m.handleEndgame(LOSE); 
		m.renderMenu(LOSE);
		break;
	case WIN:
		Mix_HaltMusic();
		state = m.handleEndgame(WIN);
		m.renderMenu(WIN);
		break;
	}	
	if (oldState == MENU && state == PLAY){
		g.gen();
		for (size_t i = 0; i < pickups.size(); i++){ pickups[i].reset(); }
		doors.clear();
		enemies.clear();
		healthDisplay.changeText(std::to_string(player->changeHealth(100, true)));
		maxHealthDisplay.changeText(std::to_string(player->changeMaxHealth(100, true)));
		
		loadLevel("output.txt", pool);
		//loadLevel("levelOne.txt", pool);
		Mix_PlayMusic(music, -1);
	}
	return state != EXIT;
}

void GameClass::animate(){
	if (lastTickCount - frameChange >= 0.1f){
	//if (lastTickCount - frameChange >= 0.4f){
		player->nextFrame();
		frameChange = lastTickCount;
	}
}

void GameClass::renderGame(){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Matrix transl;
	transl.m[3][0] = -player->getX();
	transl.m[3][1] = -player->getY();
	glMultMatrixf(transl.ml);

	for (size_t i = 0; i < beams.size(); i++){ beams[i].draw(); }

	player->draw();
	for (size_t i = 0; i < enemies.size(); i++){
		if (!inScreen(enemies[i])) { continue; }

		enemies[i].draw();
		if (enemies[i].getType() == BOSS) {
			shield.setPos(enemies[i].getX(),enemies[i].getY());
			shield.draw();
		}
	}

	for (size_t i = 0; i < doors.size(); i++){
		if (inScreen(doors[i])) { doors[i].draw(); }
	}
	
	for (size_t i = 0; i < pickups.size(); i++){
		if (inScreen(pickups[i])) { pickups[i].draw(); }
	}

	for (size_t i = 0; i < particles.size(); i++){
		if (inScreen(particles[i])) { particles[i].draw(); }
	}

	theLevel.draw();

	glLoadIdentity();
	healthDisplay.draw();
	maxHealthDisplay.draw();
	if (lastTickCount - hurtTime < 0.1f){ hurtFlash.draw();	}
	
	SDL_GL_SwapWindow(displayWindow);
}