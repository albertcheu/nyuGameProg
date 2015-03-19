#pragma once
#include "Sprite.h"

typedef struct{
	std::string name, typeName;
	float x, y;
} WhereToStart;

class Level{
public:
	Level();
	Level(const char* flareName, const char* mapName,
		int tilePix, int tileCountX, int tileCountY);
	Level(const char* flareName, TextureData td,
		int tilePix, int tileCountX, int tileCountY);

	//Resolve collisions: returns new x/y coordinate 
	float tileCollide(float x, float y, float v, float h, bool isY);

	//Render the level (center on player)
	void draw(float px, float py);

	const WhereToStart* getNext();
	
private:
	void load(const char* flareName);

	size_t whichEntity;

	//Hold the texture id of the tilemap
	TextureData td;

	//Size of tile in file; number of tiles in a row; in a column
	int tilePix, tileCountX, tileCountY;

	//How much to shift the modelview to center the level
	float offsetX, offsetY;

	//Process a filestream to get dimensions of the level, in tiles
	void loadHeader(std::ifstream& infile);
	int width, height;

	//Process a filestream to determine which tile goes where
	void loadLevel(std::ifstream& infile);
	std::vector<std::vector<int>> data;
	
	//Using level data, calculate gl- and uv-coordinates for rendering
	void fillRenderVectors();
	std::vector<float> tileVerts, tileTexts;

	//Conversion functions
	void tile2world(float* worldX, float* worldY, int tileCol, int tileRow);
	void world2tile(float worldX, float worldY, int* tileCol, int* tileRow);

	//Process a filestream to get starting locations, in xy coordinates
	void loadStarts(std::ifstream& infile);
	std::vector<WhereToStart> startLocs;
};