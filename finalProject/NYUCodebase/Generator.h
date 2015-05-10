#pragma once
#include <queue>
#include "Level.h"

const size_t LENGTH = 30;
const int NUM_LARGE = 3;

enum RoomVariant{ BLANK, LEFTSMALL, RIGHTSMALL, LEFTLARGE, RIGHTLARGE, PATH1, PATH2, QUAD };
enum ParentDir{PARENTLESS,P_LEFT,P_RIGHT};

struct Corner{
	int row, col;
};

struct Node{
	//size_t row, col;
	std::vector<size_t> neighbors;
	RoomVariant rv;
	ParentDir pd;
	Corner c, upperLeft;
};


class Generator{
public:
	Generator();
	void gen();

private:
	std::vector<Corner> upperLefts;

	std::queue<size_t> leaves;
	std::vector< Node > adj;
	std::vector<std::vector<RoomVariant> > grid;
	int gridMinRow, gridMinCol, gridMaxRow, gridMaxCol;
	Level levelSheet;

	void clear();
	void firstNodes();

	Corner fillArea(Corner c1, Corner c2, RoomVariant rv);
		bool blankArea(Corner c1, Corner c2);
		void toQuad(size_t leaf, Corner c, ParentDir pd, Corner candidateQuad);
		Corner checkQuad(Corner c, ParentDir pd);
			bool checkQuadHelper(Corner c1, Corner c2, ParentDir pd);
			std::vector<Corner> getPastQuad(Corner c1, Corner c2, ParentDir pd);

		void toPath(size_t leaf, Corner c, ParentDir pd, Corner candidatePath);
		Corner checkPath(Corner c, ParentDir pd);
		Corner getPastPath(Corner c, ParentDir pd);

		void toLarge(size_t leaf, Corner c, ParentDir pd);
		bool checkLarge(Corner c, ParentDir pd);
	
		
	void fillGrid();

	void makeFlare();
};