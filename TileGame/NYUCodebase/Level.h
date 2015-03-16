#pragma once
#include "Sprite.h"

class Level{
public:
	Level();
	Level(const char* flareName, const char* mapName,
		int tilePix, int tileCountX, int tileCountY);
	
	//Resolve collisions: returns new x/y coordinate 
	float tileCollide(float x, float y, float v, float h, bool isY);

	//Render the level (center on player)
	void draw(float px, float py);
	
private:
	//Hold the texture id of the tilemap
	TextureData td;

	//Size of tile in file; number of tiles in a row; in a column
	int tilePix, tileCountX, tileCountY;

	//How much to shift the modelview to center the level
	float offsetX, offsetY;

	//Process a filestream to get dimensions of the level, in tiles
	void readHeader(std::ifstream& infile);
	int width, height;

	//Process a filestream to determine which tile goes where
	void readLevel(std::ifstream& infile);
	std::vector<std::vector<int>> data;
	
	//Using level data, calculate gl- and uv-coordinates for rendering
	void fillVectors();
	std::vector<float> tileVerts, tileTexts;

	//Conversion functions
	void tile2world(float* worldX, float* worldY, int tileCol, int tileRow);
	void world2tile(float worldX, float worldY, int* tileCol, int* tileRow);
};