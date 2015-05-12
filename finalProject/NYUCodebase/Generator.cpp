#include "Generator.h"

void Generator::fillRoomData(RoomVariant rv, const WhereToStart* wts){
	if (wts->name == "anchor") { roomData[rv].anchor = { wts->row, wts->col }; }
	else if (wts->name == "playerStart") { roomData[rv].playerStart = { wts->row, wts->col }; }
	else if (wts->name == "pickup") { roomData[rv].pickup = { wts->row, wts->col }; }

	else if (wts->name == "enemy1") { roomData[rv].enemy1 = { wts->row, wts->col }; }
	else if (wts->name == "enemy2") { roomData[rv].enemy2 = { wts->row, wts->col }; }
	else if (wts->name == "enemy3") { roomData[rv].enemy3 = { wts->row, wts->col }; }

	else if (wts->name == "door1") { roomData[rv].door1 = { wts->row, wts->col }; }
	else if (wts->name == "door2") { roomData[rv].door2 = { wts->row, wts->col }; }
}

Generator::Generator()
	:levelSheet(Level("levelSheet.txt", "mfTRO.png", TILEPIX, TILECOUNTX, TILECOUNTY))
{
	for (int i = 0; i < 8; i++){ roomData.push_back(RoomData()); }

	const WhereToStart* wts;
	while (wts = levelSheet.getNext()){
		if (wts->typeName == "leftSmall"){ fillRoomData(LEFTSMALL, wts); }
		else if (wts->typeName == "rightSmall"){ fillRoomData(RIGHTSMALL, wts); }
		else if (wts->typeName == "leftLarge"){ fillRoomData(LEFTLARGE, wts); }
		else if (wts->typeName == "rightLarge"){ fillRoomData(RIGHTLARGE, wts); }
		else if (wts->typeName == "path1"){ fillRoomData(PATH1, wts); }
		else if (wts->typeName == "path2"){ fillRoomData(PATH2, wts); }
		else{ fillRoomData(QUAD, wts); }
		
	}
}

void Generator::clear(){
	srand(SDL_GetTicks());
	while (leaves.size()){ leaves.pop(); }
	adj.clear();
	grid.clear();
	for (int i = 0; i < LENGTH; i++){
		grid.push_back(std::vector<RoomVariant>());
		for (int j = 0; j < LENGTH; j++){
			grid[i].push_back(BLANK);
		}
	}
	data.clear();
}

void Generator::firstNodes(){
	//Always start in a small cubby
	adj.push_back(Node());
	adj[0].neighbors.push_back(1);
	adj.push_back(Node());
	adj[1].neighbors.push_back(0);

	bool top, left;
	top = rand() % 2;
	left = rand() % 2;

	if (top) { adj[0].c.row = adj[1].c.row = 0; }
	else{ adj[0].c.row = adj[1].c.row = LENGTH-1; }
	if (left) {
		adj[0].c.col = 0;
		adj[1].c.col = 1;
		adj[0].rv = LEFTSMALL;
		adj[1].rv = RIGHTSMALL;
		adj[1].pd = P_RIGHT;	
	}
	else{
		adj[0].c.col = LENGTH-1;
		adj[1].c.col = LENGTH-2;
		adj[0].rv = RIGHTSMALL;
		adj[1].rv = LEFTSMALL;
		adj[1].pd = P_LEFT;
	}

	adj[0].upperLeft = adj[0].c;
	adj[1].parent = 0;

	gridMinRow = gridMaxRow = adj[0].c.row;
	gridMinCol = gridMaxCol = adj[0].c.col;

	grid[adj[0].c.row][adj[0].c.col] = adj[0].rv;
	grid[adj[1].c.row][adj[1].c.col] = adj[1].rv;

}
bool Generator::blankArea(Corner c1, Corner c2){
	int minRow, minCol, maxRow, maxCol;
	if (c1.row < c2.row){
		minRow = c1.row;
		maxRow = c2.row;
	}
	else{
		minRow = c2.row;
		maxRow = c1.row;
	}
	if (c1.col < c2.col){
		minCol = c1.col;
		maxCol = c2.col;
	}
	else{
		minCol = c2.col;
		maxCol = c1.col;
	}
	for (int row = minRow; row < maxRow + 1; row++){
		if (row < 0 || row > LENGTH-1) { return false; }
		for (int col = minCol; col < maxCol + 1; col++){
			if (col < 0 || col > LENGTH-1) { return false; }
			//If the cell is not the starting cell and its filled
			if (!(row == c1.row && col == c1.col) && grid[row][col] != BLANK)
			{
				return false;
			}
		}
	}

	return true;
}

std::vector<Corner> Generator::getPastQuad(Corner c1, Corner c2, ParentDir pd){
	std::vector<Corner> ans;
	Corner parent = c1;
	//column determined by where we're coming from
	if (pd == P_LEFT){ parent.col++; }
	else{ parent.col--; }

	Corner left, right;
	if (c1.col < c2.col) { left = c1; right = c2; }
	else { right = c1; left = c2; }
	int cols[2] = { left.col - 1, right.col + 1 };
	int rows[2] = { c1.row, c2.row };

	for (int i = 0; i < 2; i++){
		if (rows[i] < 0 || rows[i] > LENGTH-1) { continue; }
		for (int j = 0; j < 2; j++){
			if (cols[j] < 0 || cols[j] > LENGTH-1) { continue; }
			//If connecting node is not the parent
			if (!(rows[i] == parent.row && cols[j] == parent.col)){
				//Is it blank?
				if (grid[rows[i]][cols[j]] != BLANK) { continue; }
				Corner entry = { rows[i], cols[j] };
				ans.push_back(entry);
			}			
		}
	}

	return ans;
}

Corner Generator::checkQuad(Corner c, ParentDir pd){
	Corner ans;
	//column determined by where we're coming from
	if (pd == P_LEFT){ ans.col = c.col - 1;}
	else{ ans.col = c.col + 1; }

	//Two choices for row
	int diff[2] = { -3, 3 };
	int first = rand() % 2;

	ans.row = c.row + diff[first];
	if (blankArea(c, ans) && getPastQuad(c, ans, pd).size() == 3){ return ans; }

	ans.row = c.row + diff[(first + 1) % 2];
	if (blankArea(c, ans) && getPastQuad(c, ans, pd).size() == 3){ return ans; }

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
	if (past.col < 0 || past.col > LENGTH-1) { return{ -1, -1 }; }

	if (grid[ans.row][ans.col] == BLANK && grid[past.row][past.col] == BLANK){
		return ans;
	}
	return{ -1, -1 };
}

bool Generator::checkLarge(Corner c, ParentDir pd){
	Corner ans;
	ans.row = c.row - 1;
	ans.col = c.col + 1;
	//column determined by where we're coming from
	if (pd == P_LEFT){ ans.col = c.col - 1; }
	return blankArea(c, ans);
}

void Generator::toPath(size_t leaf, Corner c, ParentDir pd, Corner candidatePath){
	size_t node = adj.size();
	Corner past = getPastPath(c, pd);
	adj.push_back(Node());
	adj[node].c = adj[node].upperLeft = past;
	adj[node].pd = pd;
	adj[node].rv = adj[leaf].rv;
	adj[node].parent = leaf;

	adj[node].neighbors.push_back(leaf);
	adj[leaf].neighbors.push_back(node);
	adj[leaf].rv = ((rand() % 2) ? PATH2 : PATH1);
	adj[leaf].upperLeft = fillArea(c, candidatePath, adj[leaf].rv);
	grid[past.row][past.col] = ((pd == P_LEFT)?LEFTSMALL:RIGHTSMALL);

	leaves.push(node);
	if (adj[node].c.col < gridMinCol){ gridMinCol = adj[node].c.col; }
	if (adj[node].c.col > gridMaxCol){ gridMaxCol = adj[node].c.col; }
}

void Generator::toLarge(size_t leaf, Corner c, ParentDir pd){
	int row = c.row;
	int col = c.col;
	if (adj[leaf].rv == LEFTSMALL){
		adj[leaf].rv = LEFTLARGE;
		grid[row][col] = grid[row][col-1] = grid[row-1][col-1] = grid[row-1][col] = LEFTLARGE;
		adj[leaf].upperLeft = { row - 1, col - 1 };
		if (col - 1 < gridMinCol){ gridMinCol = col - 1; }
	}
	else{
		adj[leaf].rv = RIGHTLARGE;
		grid[row][col] = grid[row][col + 1] = grid[row - 1][col + 1] = grid[row - 1][col] = RIGHTLARGE;
		adj[leaf].upperLeft = { row - 1, col };
		if (col + 1 > gridMaxCol){ gridMaxCol = col + 1; }
	}
	if (row - 1 < gridMinRow){ gridMinRow = row - 1; }
}

void Generator::toQuad(size_t leaf, Corner c, ParentDir pd, Corner candidateQuad){
	adj[leaf].upperLeft = fillArea(c, candidateQuad,QUAD);
	adj[leaf].rv = QUAD;
	std::vector<Corner> newLocs = getPastQuad(c, candidateQuad, pd);
	for (size_t i = 0; i < 3; i++){
		size_t node = adj.size();
		adj.push_back(Node());
		adj[leaf].neighbors.push_back(node);
		adj[node].neighbors.push_back(leaf);
		adj[node].parent = leaf;
		adj[node].c = adj[node].upperLeft = newLocs[i];
		if (adj[node].c.col > adj[leaf].c.col){
			adj[node].rv = RIGHTSMALL;
			adj[node].pd = P_RIGHT;
		}
		else{
			adj[node].rv = LEFTSMALL;
			adj[node].pd = P_LEFT;
		}
		grid[adj[node].c.row][adj[node].c.col] = adj[node].rv;
		leaves.push(node);

		if (adj[node].c.row < gridMinRow){ gridMinRow = adj[node].c.row; }
		if (adj[node].c.row > gridMaxRow){ gridMaxRow = adj[node].c.row; }
		if (adj[node].c.col < gridMinCol){ gridMinCol = adj[node].c.col; }
		if (adj[node].c.col > gridMaxCol){ gridMaxCol = adj[node].c.col; }
	}
}

Corner Generator::fillArea(Corner c1, Corner c2, RoomVariant rv){
	size_t minRow, minCol, maxRow, maxCol;
	if (c1.row < c2.row){
		minRow = c1.row;
		maxRow = c2.row;
	}
	else{
		minRow = c2.row;
		maxRow = c1.row;
	}
	if (c1.col < c2.col){
		minCol = c1.col;
		maxCol = c2.col;
	}
	else{
		minCol = c2.col;
		maxCol = c1.col;
	}
	for (size_t row = minRow; row < maxRow + 1; row++){
		for (size_t col = minCol; col < maxCol + 1; col++){
			grid[row][col] = rv;
		}
	}
	return{ minRow, minCol };
}

void Generator::fillData(){
	//Convert our grid coordinates to tile coordinates: offset and scale
	for (size_t i = 0; i < adj.size(); i++){
		adj[i].upperLeft.row -= gridMinRow;
		adj[i].upperLeft.col -= gridMinCol;
		adj[i].upperLeft.row *= 8;
		adj[i].upperLeft.col *= 8;

		adj[i].c.row -= gridMinRow;
		adj[i].c.col -= gridMinCol;
		adj[i].c.row *= 8;
		adj[i].c.col *= 8;

	}

	//Initialize our tile data as an empty matrix
	int width = gridMaxCol - gridMinCol + 1;
	int height = gridMaxRow - gridMinRow + 1;
	for (int row = 0; row < height * 8; row++){//Each cell in grid stands for 8x8 tiles
		data.push_back(std::vector<int>());
		for (int col = 0; col < width * 8; col++){
			data[row].push_back(0);
		}
	}

	//Fill tile data using the levelSheet
	for (size_t i = 0; i < adj.size(); i++){
		int row = adj[i].upperLeft.row;
		int col = adj[i].upperLeft.col;

		int roomWidth = 2;
		int roomHeight = 4;
		if (adj[i].rv == LEFTLARGE || adj[i].rv == RIGHTLARGE){ roomHeight = 2; }
		else if (adj[i].rv == PATH1 || adj[i].rv == PATH2){ roomHeight = 1; }
		else if (adj[i].rv == LEFTSMALL || adj[i].rv == RIGHTSMALL){
			roomWidth = roomHeight = 1;
		}
		roomWidth *= 8;
		roomHeight *= 8;

		for (int tileRow = row; tileRow < row + roomHeight; tileRow++){
			for (int tileCol = col; tileCol < col + roomWidth; tileCol++){
				int templateRow = roomData[adj[i].rv].anchor.row + (tileRow - row);
				int templateCol = roomData[adj[i].rv].anchor.col + (tileCol - col);
				data[tileRow][tileCol] = levelSheet.data[templateRow][templateCol];
			}
		}
	}
}

void Generator::fillGrid(){
	size_t minNumLeaves = (NUMTANKS+3) + (rand() % 7);
	size_t numLeaves = 1;
	leaves.push(1);

	while (numLeaves < minNumLeaves && leaves.size()){
		size_t leaf = leaves.front();
		leaves.pop();

		ParentDir pd = adj[leaf].pd;
		Corner c = adj[leaf].c;

		Corner candidatePath = checkPath(c, pd);
		if (candidatePath.col > -1){

			Corner candidateQuad = checkQuad(c, pd);
			if (candidateQuad.col > -1){
				if (rand() % 3) {
					toQuad(leaf, c, pd, candidateQuad);
					numLeaves += 2;
				}
				else{ toPath(leaf, c, pd, candidatePath); }

			}

			//can only be path
			else{ toPath(leaf, c, pd, candidatePath); }
		}

		//can't do anything but assert leafhood
		else{ adj[leaf].upperLeft = adj[leaf].c; }
	}

	int counter = 0;
	for (size_t i = 0; i < adj.size(); i++){
		Corner c = adj[i].c;
		if (adj[i].neighbors.size() == 1 && checkLarge(c, adj[i].pd)){
			counter++;
			toLarge(i, c, adj[i].pd);
		}
		if (counter == NUM_LARGE) { break; }
	}
}

Corner Generator::getFinalCoor(size_t i, Corner target){
	//Player: offset by anchor and then by upper left of node
	Corner a = roomData[adj[i].rv].anchor;
	int row = target.row - a.row + adj[i].upperLeft.row;
	int col = target.col - a.col + adj[i].upperLeft.col;
	return{ row, col };
}

void Generator::traverseUp(size_t i, size_t pathType){
	size_t parent = -1;
	while (i != 0 && !(adj[i].visited) ){
		parent = adj[i].parent;

		Node* leftRoom = &(adj[i]);
		Node* rightRoom = &(adj[parent]);
		if (adj[i].upperLeft.col > adj[parent].upperLeft.col){
			leftRoom = &(adj[parent]);
			rightRoom = &(adj[i]);
		}

		BeamColor* bc = &(leftRoom->color1);
		if (leftRoom->rv == QUAD && leftRoom->upperLeft.row < rightRoom->upperLeft.row){
			bc = &(leftRoom->color2);
		}

		if (pathType == 0){
			if (adj[i].neighbors.size() == 1) { *bc = BLUE; }
			else { *bc = (BeamColor)(rand() % 4); }
		}
		
		//path type = Yellow, green, or blue (1,2,3)
		else if (pathType > RED && pathType <= BLUE){
			if (adj[i].neighbors.size() == 1) { *bc = (BeamColor)(pathType-1); }
			else { *bc = (BeamColor)(rand() % pathType); }
		}

		//Energy tank or empty
		else { *bc = (BeamColor)(rand() % 4); }

		adj[i].visited = true;

		i = parent;
	}
}

void Generator::writeDoors(std::ofstream& ofs){
	Corner f;
	for (size_t i = 0; i < adj.size(); i++){
		std::string doorColor = "red";
		if (adj[i].color1 == YELLOW) { doorColor = "yellow"; }
		else if (adj[i].color1 == GREEN) { doorColor = "green"; }
		else if (adj[i].color1 == BLUE) { doorColor = "blue"; }

		//Doors are "lefthanded" (only the left half is stored)
		if (adj[i].rv != RIGHTLARGE || adj[i].rv != RIGHTSMALL){
			f = getFinalCoor(i, roomData[adj[i].rv].door1);
			writeObject(ofs, std::to_string(i), doorColor, f.row, f.col);
		}		

		if (adj[i].rv == QUAD){
			doorColor = "red";
			f = getFinalCoor(i, roomData[adj[i].rv].door2);
			if (adj[i].color2 == YELLOW) { doorColor = "yellow"; }
			else if (adj[i].color2 == GREEN) { doorColor = "green"; }
			else if (adj[i].color2 == BLUE) { doorColor = "blue"; }
			writeObject(ofs, std::to_string(i), doorColor, f.row, f.col);
		}
	}
}

void Generator::writeObject(std::ofstream& ofs, std::string name, std::string typeName,
	int row, int col){
	ofs << "\n[StartLocations]\n# " << name << "\ntype=" << typeName << std::endl;
	ofs << "location=" << col << ',' << row << ",0,0" << std::endl;
}

void Generator::gen(){
	clear();
	firstNodes();
	fillGrid();
	fillData();
	
	std::ofstream ofs("output.txt");
	ofs << "[header]\nwidth=" << data[0].size() << "\nheight=" << data.size() << std::endl;
	ofs << "tilewidth=16\ntileHeight=16\norientation=orthogonal" << std::endl;
	ofs << "\n[tilesets]\ntileset=mfTRO.png,16,16,0,0" << std::endl;
	ofs << "\n[layer]\ntype=Tile Layer 1\ndata=" << std::endl;

	for (size_t i = 0; i < data.size(); i++){
		for (size_t j = 0; j < data[i].size(); j++){ ofs << data[i][j] + 1 << ','; }
		ofs << std::endl;
	}
	
	Corner f = getFinalCoor(0, roomData[adj[0].rv].playerStart);
	writeObject(ofs, "ps", "PlayerStart", f.row, f.col);
		
	for (size_t i = 0; i < adj.size(); i++){
		adj[i].visited = (i == 0);
		adj[i].color1 = adj[i].color2 = RED;
	}
	size_t pickupCounter = 1;
	bool putBoss = false;
	size_t whereBoss = -1;
	for (size_t i = 1; i < adj.size(); i++){
		//Cubby - one way in/out
		if (adj[i].neighbors.size() == 1){
			if (!putBoss && (adj[i].rv == LEFTLARGE || adj[i].rv == RIGHTLARGE)){
				putBoss = true;
				f = getFinalCoor(i, roomData[adj[i].rv].enemy1);
				writeObject(ofs, std::to_string(i), "boss", f.row, f.col);
				whereBoss = i;
				continue;
			}
			std::string name = "tank";
			if (pickupCounter == YELLOW){ name = "yellow"; }
			else if (pickupCounter == GREEN) { name = "green"; }
			else if (pickupCounter == BLUE) { name = "blue"; }
			traverseUp(i, pickupCounter);

			if (pickupCounter <= NUMTANKS + 3){
				pickupCounter++;
				f = getFinalCoor(i, roomData[adj[i].rv].pickup);
				writeObject(ofs, name, "pickup", f.row, f.col);
			}
		}

		//Path - joins two other rooms
		else if (adj[i].neighbors.size() == 2){
			if (rand() % 2){
				f = getFinalCoor(i, roomData[adj[i].rv].enemy1);
				std::string type = (rand() % 2) ? "runner" : "hopper";
				writeObject(ofs, std::to_string(i), type, f.row, f.col);
			}
		}

		else if (adj[i].neighbors.size() == 4){
			//enemy1 - must be flier or nothing
			if (rand() % 2){
				f = getFinalCoor(i, roomData[adj[i].rv].enemy1);
				writeObject(ofs, std::to_string(i), "flier", f.row, f.col);
			}
			//2,3 runner or nothing
			if (rand() % 2){
				f = getFinalCoor(i, roomData[adj[i].rv].enemy2);
				writeObject(ofs, std::to_string(i), "runner", f.row, f.col);
			}
			if (rand() % 2){
				f = getFinalCoor(i, roomData[adj[i].rv].enemy3);
				writeObject(ofs, std::to_string(i), "runner", f.row, f.col);
			}
		}
	}
	traverseUp(whereBoss, 0);
	writeDoors(ofs);

	ofs.close();
}
