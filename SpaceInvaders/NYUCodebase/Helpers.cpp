#include "Helpers.h"

unsigned getKey(){
	SDL_Event event;
	//Enable closing of window
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) { return CLOSE_WINDOW; }
		else if (event.type == SDL_KEYDOWN){
			//If we press escape, pause
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){ return KEY_ESCAPE; }
			else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){ return KEY_SPACE; }
		}
	}
	return OTHER;
}
/*
void renderPause(SDL_Window* displayWindow){

}

void renderStart(SDL_Window* displayWindow){

}
void renderWin(SDL_Window* displayWindow){

}
void renderLose(SDL_Window* displayWindow){

}
*/