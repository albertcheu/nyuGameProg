#pragma once
#include "Entity.h"

class Menu{
public:
	Menu();
	Menu(SDL_Window* displayWindow, Mix_Chunk *switchSound,
		Mix_Chunk *move, Mix_Chunk* bump);

	GameState handleEvents();
	GameState handleEndgame(GameState whichState);
	void renderMenu(GameState whichState);
private:
	size_t whichOption, whichSprite;
	Text exitIcon, leftIcon, rightIcon;
	Entity selector;
	std::vector<Sprite> sprites;
	Sprite loseCard, winCard;
	SDL_Window* displayWindow;
	Mix_Chunk *move, *switchSound, *bump;

	GameState select();
};