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
				break;
			case SDL_SCANCODE_RETURN:
				break;
			case SDL_SCANCODE_LEFT:
				break;
			case SDL_SCANCODE_RIGHT:
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
void Menu::renderMenu(){
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//475x283, 418x283
	float width = 1.4f*(whichSprite == 0 ? 475.0f / 283.0f : 418.0f / 283.0f);
	sprites[whichSprite].draw(width,1.4f);

	selector.setY(0.16f-0.3f*whichOption);
	selector.draw();

	SDL_GL_SwapWindow(displayWindow);
}