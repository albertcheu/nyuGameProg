#include "Helpers.h"

float lerp(float v0, float v1, float t) {
	return (1.0f - t)*v0 + t*v1;
}

float depenetrate(float c1, float h1, float c2, float h2){
	float p = fabs(fabs(c1 - c2) - h1 - h2);
	float d = p + 0.001f;
	if (c1 < c2){ d *= -1; }
	return c1 + d;
}

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