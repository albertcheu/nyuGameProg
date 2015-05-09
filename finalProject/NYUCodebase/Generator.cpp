#include "Generator.h"

Generator::Generator()
	//:levelSheet(Level("levelSheet.txt", "mfTRO.png", TILEPIX, TILECOUNTX, TILECOUNTY))
{}

void Generator::gen(){
	while (leaves.size()){ leaves.pop(); }
	adj.clear();

	//Always start in a cubby (large or small)
	adj.push_back(std::vector<int>());
	adj.push_back(std::vector<int>());
	adj[0].push_back(1);
	adj[1].push_back(0);

	size_t minNumLeaves = 10 + (rand() % 7);
	int leaf = 1;
	leaves.push(leaf++);

	while (leaves.size() < minNumLeaves){
		int currentNode = leaves.front();
		leaves.pop();

		//0, 1, or 3 children (rooms have 1, 2, or 4 doors)
		size_t numChildren = rand() % 3;
		if (numChildren == 2) { numChildren++; }

		if (numChildren == 0){ leaves.push(currentNode); }
		else{
			while (numChildren > 0){
				while (leaf >= adj.size()) { adj.push_back(std::vector<int>()); }

				adj[currentNode].push_back(leaf);
				adj[leaf].push_back(currentNode);
				
				leaves.push(leaf++);
				numChildren--;
			}
		}
	}
	OutputDebugString("Made tree");
	for (size_t i = 0; i < adj.size(); i++){
		std::string s = std::to_string(i) + ": ";
		for (size_t j = 0; j < adj[i].size(); j++){
			s += std::to_string(adj[i][j]) + " ";
		}
		OutputDebugString(s.c_str());
	}
}