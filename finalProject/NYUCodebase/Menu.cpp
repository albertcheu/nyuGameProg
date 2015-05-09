#include "Menu.h"

Menu::Menu(){}
Menu::Menu(SDL_Window* displayWindow)
	:displayWindow(displayWindow), whichOption(0), whichSprite(0)
{
	TextureData td = LoadTextureRGB("menus/menu.png");
	sprites.push_back(Sprite(td.id, 0, 0, 1, 1));

	td = LoadTextureRGB("menus/controls.png");
	sprites.push_back(Sprite(td.id, 0, 0, 1, 1));
	
	td = LoadTextureRGB("menus/objective.png");
	sprites.push_back(Sprite(td.id, 0, 0, 1, 1));
	
	td = LoadTextureRGB("menus/enemies.png");
	sprites.push_back(Sprite(td.id, 0, 0, 1, 1));
	
	td = LoadTextureRGB("menus/health.png");
	sprites.push_back(Sprite(td.id, 0, 0, 1, 1));
	
	td = LoadTextureRGB("menus/beamsAndDoors.png");
	sprites.push_back(Sprite(td.id, 0, 0, 1, 1));

	td = LoadTextureRGBA("sprites/bossBeam.png");
	selector = Entity(-0.3f,0,0.1f,0.1f,Sprite(td.id, 0, 0, 1, 1));

	td = LoadTextureRGBA("sprites/font.png");
	exitIcon = Text(td.id, 1.0f / 16.0f, 0.05f, -1, 0.8f, "ESC");
	leftIcon = Text(td.id, 1.0f / 16.0f, 0.13f, -1.1f, 0, "<");
	rightIcon = Text(td.id, 1.0f / 16.0f, 0.13f, 1.1f, 0, ">");

	td = LoadTextureRGB("menus/lose.png");
	loseCard = Sprite(td.id, 0, 0, 1, 1);
	td = LoadTextureRGB("menus/win.png");
	winCard = Sprite(td.id, 0, 0, 1, 1);
}
GameState Menu::select(){
	switch (whichOption){
	case 0:
		return PLAY;
	case 1:
		whichSprite++;
		return MENU;
	case 2:
		return EXIT;
	}
}
GameState Menu::handleEvents(){
	GameState ans = MENU;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			ans = EXIT;
		}
		else if (event.type == SDL_KEYDOWN){
			switch (event.key.keysym.scancode){
			case SDL_SCANCODE_SPACE:
				if (whichSprite == 0){ ans = select(); }
				break;
			case SDL_SCANCODE_RETURN:
				if (whichSprite == 0){ ans = select(); }
				break;
			case SDL_SCANCODE_LEFT:
				if (whichSprite > 1){ whichSprite--; }
				break;
			case SDL_SCANCODE_RIGHT:
				if (whichSprite < sprites.size() - 1){ whichSprite++; }
				break;
			case SDL_SCANCODE_UP:
				if (whichSprite == 0){
					whichOption = (whichOption - 1) % 3;
				}
				break;
			case SDL_SCANCODE_DOWN:
				if (whichSprite == 0){
					whichOption = (whichOption + 1) % 3;
				}
				break;
			case SDL_SCANCODE_ESCAPE:
				if (whichSprite > 0){ whichSprite = 0; }
				break;
			}
		}
	}
	return ans;
}
GameState Menu::handleEndgame(GameState whichState){
	GameState ans = whichState;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			ans = EXIT;
		}
		else if (event.type == SDL_KEYDOWN &&
			event.key.keysym.scancode==SDL_SCANCODE_ESCAPE){
			ans = MENU;
		}
	}
	return ans;
}
void Menu::renderMenu(GameState whichState){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float width;
	switch (whichState){
	case MENU:
		//475x283, 418x283
		width = 1.4f*(whichSprite == 0 ? 475.0f / 283.0f : 418.0f / 283.0f);
		sprites[whichSprite].draw(width, 1.4f);
		if (whichSprite == 0){
			selector.setY(0.16f - 0.3f*whichOption);
			selector.draw();
		}
		else{
			exitIcon.draw();
			if (whichSprite > 1) { leftIcon.draw(); }
			if (whichSprite < sprites.size() - 1) { rightIcon.draw(); }
		}
		break;
	case LOSE:
		loseCard.draw(418.0f / 283.0f, 1.4f);
		break;
	case WIN:
		winCard.draw(418.0f / 283.0f, 1.4f);
		break;
	}
	SDL_GL_SwapWindow(displayWindow);
}