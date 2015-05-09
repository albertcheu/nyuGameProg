#include "Generator.h"

Generator::Generator()
	//:levelSheet(Level("levelSheet.txt", "mfTRO.png", TILEPIX, TILECOUNTX, TILECOUNTY))
{}

void Generator::setup(){
	srand(SDL_GetTicks());
	while (leaves.size()){ leaves.pop(); }
	adj.clear();
	grid.clear();
	for (int i = 0; i < 64; i++){
		grid.push_back(std::vector<RoomVariant>());
		for (int j = 0; j < 64; j++){
			grid[i].push_back(BLANK);
		}
	}
}

void Generator::firstNodes(){
	//Always start in a small cubby
	adj.push_back(Node());
	adj[0].neighbors.push_back(1);
	adj.push_back(Node());
	adj[1].neighbors.push_back(0);

	if (rand() % 2) { adj[0].row = 0; adj[1].row = 1; }
	else{ adj[0].row = 63; adj[1].row = 62; }
	if (rand() % 2) {
		adj[0].col = 0;
		adj[1].col = 1;
		adj[0].rv = LEFTSMALL;
		adj[1].rv = RIGHTSMALL;
		adj[1].pd = P_RIGHT;
	}
	else{
		adj[0].col = 63;
		adj[1].col = 62;
		adj[0].rv = RIGHTSMALL;
		adj[1].rv = LEFTSMALL;
		adj[1].pd = P_LEFT;
	}
	grid[adj[0].row][adj[0].col] = adj[0].rv;
	grid[adj[1].row][adj[1].col] = adj[1].rv;
}
Corners Generator::checkQuad(size_t leaf){

}
Corners Generator::checkPath(size_t leaf){

}
Corners Generator::checkLarge(size_t leaf){

}

void Generator::gen(){
	setup();

	firstNodes();
	
	size_t minNumLeaves = 10 + (rand() % 7);
	size_t numLeaves = 1;
	leaves.push(1);

	while (numLeaves < minNumLeaves && leaves.size()){
		size_t leaf = leaves.front();
		leaves.pop();
		Corners quad, path, large;
		quad = checkQuad(leaf); path = checkPath(leaf); large = checkLarge(leaf);

	}
}
