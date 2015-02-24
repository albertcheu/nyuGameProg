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

StateAndRun updateStart(){
	StateAndRun ans = { START, true };
	switch (getKey()){
	case KEY_ESCAPE: break;
	case CLOSE_WINDOW:
		ans.keepRunning = false;
		break;
	case OTHER: break;
	case KEY_SPACE:
		ans.newState = GAME;
		break;
	}
	return ans;
}

StateAndRun updatePause(){
	StateAndRun ans = { PAUSE, true };
	switch (getKey()){
	case KEY_ESCAPE:
		ans.newState = GAME;
		break;
	case CLOSE_WINDOW:
		ans.keepRunning = false;
		break;
	case OTHER: break;
	case KEY_SPACE:break;
	}
	return ans;
}

StateAndRun updateWin(){
	StateAndRun ans = { WIN, true };
	switch (getKey()){
	case KEY_ESCAPE: break;
	case CLOSE_WINDOW:
		ans.keepRunning = false;
		break;
	case OTHER: break;
	case KEY_SPACE:
		ans.newState = GAME;
		break;
	}
	return ans;
}

StateAndRun updateLose(){
	StateAndRun ans = { LOSE, true };
	switch (getKey()){
	case KEY_ESCAPE: break;
	case CLOSE_WINDOW:
		ans.keepRunning = false;
		break;
	case OTHER: break;
	case KEY_SPACE:
		ans.newState = GAME;
		break;
	}
	return ans;
}