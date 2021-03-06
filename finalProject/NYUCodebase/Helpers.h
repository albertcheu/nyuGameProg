#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <math.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define UNIT_WIDTH 1.33f
#define UNIT_HEIGHT 1.0f
#define TILEUNITS 0.07f//size of a tile in OpenGL units
#define TILEPIX 16//pixel size of a tile in mfTRO
#define TILECOUNTX 16//number of tiles in a row in mfTRO
#define TILECOUNTY 24//number of tiles in a column in mfTRO

enum BeamColor{ RED, YELLOW, GREEN, BLUE };
enum GameState{ MENU, PLAY, LOSE, WIN, EXIT };

float dist(float x1, float y1, float x2, float y2);

float lerp(float v0, float v1, float t);
float depenetrate(float c1, float h1, float c2, float h2);

bool isSolid(int t, const char* mapName);