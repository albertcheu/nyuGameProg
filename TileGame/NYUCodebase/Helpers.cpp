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

void readHeader(std::ifstream &stream, int* width, int* height, int*** level){
	std::string line;
	
	while (getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") { *width = atoi(value.c_str()); }
		else if (key == "height"){ *height = atoi(value.c_str()); }
	}

	// allocate our map data
	*level = new int*[*height];
	for (int i = 0; i < *height; ++i) { (*level)[i] = new int[*width]; }
}

void freeLevel(int height, int*** level){
	for (int i = 0; i < height; i++) { delete[] (*level)[i]; }
	delete[] (*level);
}

void readLayerData(std::ifstream &stream, int width, int height, int*** level){
	std::string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < height; y++) {
				getline(stream, line);
				std::istringstream lineStream(line);
				std::string tile;
				for (int x = 0; x < width; x++) {
					getline(lineStream, tile, ',');
					int val = atoi(tile.c_str());
					
					// be careful, the tiles in this format are indexed from 1 not 0
					if (val > 0) { (*level)[y][x] = val - 1; }
					else {	(*level)[y][x] = 0; }
					
				}
			}
		}
	}
}

void loadLevel(const char* levelFile, int* width, int* height, int*** level){
	std::ifstream infile(levelFile);
	std::string line;
	while (getline(infile, line)) {
		if (line == "[header]") { readHeader(infile, width, height, level); }
		else if (line == "[layer]") { readLayerData(infile, *width, *height, level); }
		else if (line == "[StartLocations]") {
			//To do
			//readEntityData(infile);
		}
	}
	infile.close();
}
