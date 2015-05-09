#include "Generator.h"

Generator::Generator()
	//:levelSheet(Level("levelSheet.txt", "mfTRO.png", TILEPIX, TILECOUNTX, TILECOUNTY))
{}

void Generator::clear(){
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
bool Generator::blankArea(Corner c1, Corner c2){
	int rowDel = ((c2.row >= c1.row) ? 1 : -1);
	int colDel = ((c2.col >= c1.col) ? 1 : -1);
	
	for (size_t row = c1.row; row <= c2.row; row += rowDel){
		if (row < 0 || row > 63) { return false; }

		for (size_t col = c1.col; col <= c2.col; col += colDel){
			if (col < 0 || col > 63) { return false; }

			//If the cell is not the starting cell and its filled
			if (!(row == c1.row && col == c1.col) && grid[row][col] != BLANK)
				{ return false; }
		}
	}
	
	return true;
}

bool Generator::checkQuadHelper(Corner c1, Corner c2, Corner parent){
	Corner left, right;
	if (c1.col < c2.col) { left = c1; right = c2; }
	else { right = c1; left = c2; }
	int cols[2] = { left.col - 1, right.col + 1 };
	int rows[2] = { c1.row, c2.row };

	for (int i = 0; i < 2; i++){
		if (rows[i] < 0 || rows[i] > 63) { return false; }
		for (int j = 0; j < 2; j++){
			if (cols[j] < 0 || cols[j] > 63) { return false; }
			if (!(rows[i] == parent.row && cols[j]==parent.col)
				&& grid[rows[i]][cols[j]] != BLANK) { return false; }
		}
	}
	return true;
}

Corner Generator::checkQuad(Corner c, ParentDir pd){
	Corner parent = c;
	Corner ans;
	//column determined by where we're coming from
	if (pd == P_LEFT){ ans.col = c.col - 1; parent.col++; }
	else{ ans.col = c.col + 1; parent.col--; }

	//Two choices for row
	int diff[2] = { -3, 3 };
	int first = rand() % 2;

	ans.row = c.row + diff[first];
	if (blankArea(c, ans) && checkQuadHelper(c, ans, parent)){
		return ans;
	}

	ans.row = c.row + diff[(first + 1) % 2];
	if (blankArea(c, ans) && checkQuadHelper(c, ans, parent)){
		return ans;
	}

	ans.row = -1; ans.col = -1;
	return ans;
}

Corner Generator::getPastPath(Corner c, ParentDir pd){
	Corner past = c;
	if (pd == P_LEFT) { past.col -= 2; }
	else { past.col += 2; }
	return past;
}

Corner Generator::checkPath(Corner c, ParentDir pd){
	Corner ans = c;
	Corner past = getPastPath(c,pd);
	if (pd == P_LEFT) { ans.col--; }
	else { ans.col++; }
	if (past.col < 0 || past.col > 63) { return{ -1, -1 }; }

	if (grid[ans.row][ans.col] == BLANK && grid[past.row][past.col] == BLANK){
		return ans;
	}
	return{ -1, -1 };
}

bool Generator::checkLarge(Corner c, ParentDir pd){
	Corner ans;
	//column determined by where we're coming from
	if (pd == P_LEFT){ ans.col = c.col - 1; }
	else{ ans.col = c.col + 1; }
	ans.row--;
	return blankArea(c, ans);
}

void Generator::toPath(size_t leaf, Corner c, ParentDir pd, Corner candidatePath){
	size_t node = adj.size();
	Corner past = getPastPath(c, pd);
	adj.push_back(Node());
	adj[node].row = past.row;
	adj[node].col = past.col;
	adj[node].pd = pd;
	adj[node].rv = adj[leaf].rv;
	
	adj[node].neighbors.push_back(leaf);
	adj[leaf].neighbors.push_back(node);
	adj[leaf].rv = ((rand() % 2) ? PATH2 : PATH1);
	fillArea(c, candidatePath, adj[leaf].rv);
	grid[past.row][past.col] = adj[node].rv;
	leaves.push(node);
}

void Generator::toLarge(size_t leaf, Corner c, ParentDir pd){
	size_t row = c.row;
	size_t col = c.col;
	if (adj[leaf].rv == LEFTSMALL){
		adj[leaf].rv = LEFTLARGE;
		grid[row][col] = grid[row][col-1] = grid[row-1][col-1] = grid[row-1][col] = LEFTLARGE;
	}
	else{
		adj[leaf].rv = RIGHTLARGE;
		grid[row][col] = grid[row][col + 1] = grid[row - 1][col + 1] = grid[row - 1][col] = RIGHTLARGE;
	}
}

void Generator::gen(){
	clear();
	firstNodes();
	
	size_t minNumLeaves = 10 + (rand() % 7);
	size_t numLeaves = 1;
	leaves.push(1);

	while (numLeaves < minNumLeaves && leaves.size()){
		size_t leaf = leaves.front();
		leaves.pop();

		size_t row = adj[leaf].row;
		size_t col = adj[leaf].col;
		ParentDir pd = adj[leaf].pd;
		Corner c = { row, col };

		Corner candidatePath = checkPath(c, pd);
		if (candidatePath.col > -1){
			
			if (checkLarge(c, pd)){
				Corner candidateQuad = checkQuad(c, pd);
				if (candidateQuad.col > -1){
					//path, large leaf, or quad
				}
				else{
					//path or large leaf
					(rand() % 2) ? toPath(leaf, c, pd, candidatePath) : toLarge(leaf, c, pd);
				}
			}

			//can only be path
			else{ toPath(leaf, c, pd, candidatePath); }			
		}
		else{
			//can't do anything
		}		
	}
}
