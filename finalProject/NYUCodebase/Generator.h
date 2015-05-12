#pragma once
#include <queue>
#include "Level.h"

const int NUMTANKS = 6;
const size_t LENGTH = 30;
const int NUM_LARGE = 3;

enum RoomVariant{ BLANK, LEFTSMALL, RIGHTSMALL, LEFTLARGE, RIGHTLARGE, PATH1, PATH2, QUAD };
enum ParentDir{PARENTLESS,P_LEFT,P_RIGHT};

struct Corner{
	int row, col;
};

struct Node{
	std::vector<size_t> neighbors;
	RoomVariant rv;
	ParentDir pd;
	Corner c, upperLeft;
	bool visited;
	BeamColor color1, color2;
	size_t parent;
};

struct RoomData{
	Corner anchor, playerStart, pickup, enemy1, enemy2, enemy3, door1, door2;	
};

class Generator{
public:
	Generator();
	void gen();

private:
	Level levelSheet;
	std::vector<RoomData> roomData;
	void fillRoomData(RoomVariant rv, const WhereToStart* wts);

	std::queue<size_t> leaves;
	std::vector< Node > adj;
	std::vector<std::vector<RoomVariant> > grid;
	int gridMinRow, gridMinCol, gridMaxRow, gridMaxCol;

	std::vector<std::vector<int> > data;

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

	void fillData();

	Corner getFinalCoor(size_t i, Corner target);
	void traverseUp(size_t i, size_t pathType);
	void writeDoors(std::ofstream& ofs);
	void writeObject(std::ofstream& ofs, std::string name, std::string typeName,
		int row, int col);
};