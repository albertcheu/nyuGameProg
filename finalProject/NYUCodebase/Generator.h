#pragma once
#include <queue>
#include "Level.h"

enum RoomVariant{ BLANK, LEFTSMALL, RIGHTSMALL, LEFTLARGE, RIGHTLARGE, PATH1, PATH2, QUAD };
enum ParentDir{PARENTLESS,P_LEFT,P_RIGHT};

struct Node{
	size_t row, col;
	std::vector<size_t> neighbors;
	RoomVariant rv;
	ParentDir pd;
};

struct Corners{
	struct { size_t topRight, topLeft, botRight, botLeft; }x;
	struct { size_t topRight, topLeft, botRight, botLeft; }y;
};

class Generator{
public:
	Generator();
	void gen();

private:
	std::queue<size_t> leaves;
	std::vector< Node > adj;
	std::vector<std::vector<RoomVariant> > grid;

	void setup();
	void firstNodes();

	Corners checkQuad(size_t leaf);
	Corners checkPath(size_t leaf);
	Corners checkLarge(size_t leaf);

	Level levelSheet;
	
	void makeFlare();
};