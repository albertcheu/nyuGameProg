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

struct Corner{
	int row, col;
};

class Generator{
public:
	Generator();
	void gen();

private:
	std::queue<size_t> leaves;
	std::vector< Node > adj;
	std::vector<std::vector<RoomVariant> > grid;

	void clear();
	void firstNodes();

	bool blankArea(Corner c1, Corner c2);
	Corner checkQuad(Corner c, ParentDir pd);
	bool checkQuadHelper(Corner c1, Corner c2, Corner parent);
	void fillArea(Corner c1, Corner c2, RoomVariant rv);

	Corner getPastPath(Corner c, ParentDir pd);
	Corner checkPath(Corner c, ParentDir pd);
	bool checkLarge(Corner c, ParentDir pd);

	void toPath(size_t leaf, Corner c, ParentDir pd, Corner candidatePath);
	void toLarge(size_t leaf, Corner c, ParentDir pd);

	Level levelSheet;
	
	void makeFlare();
};