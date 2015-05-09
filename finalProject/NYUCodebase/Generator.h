#pragma once
#include <queue>
#include "Level.h"

const int SMALL_WIDTH = 8;
const int SMALL_HEIGHT = 7;

const int PATH_WIDTH = 18;
const int PATH_HEIGHT = 9;

const int QUAD_WIDTH = 16;
const int QUAD_HEIGHT = 26;

const int LARGE_WIDTH = 27;
const int LARGE_HEIGHT = 14;

class Generator{
public:
	Generator();
	void gen();

private:
	std::queue<int> leaves;
	std::vector<std::vector<int> > adj;
	Level levelSheet;
	
	void makeFlare();
};