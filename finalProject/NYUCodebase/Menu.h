#pragma once
#include "Entity.h"

class Menu{
public:
	Menu();
	Menu(SDL_Window* displayWindow);
	GameState handleEvents();
	void renderMenu();
private:
	size_t whichOption, whichSprite;
	Text exitIcon, leftIcon, rightIcon;
	Entity selector;
	std::vector<Sprite> sprites;
	SDL_Window* displayWindow;

	GameState select();
};