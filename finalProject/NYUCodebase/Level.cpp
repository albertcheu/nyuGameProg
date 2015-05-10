#include "Level.h"

Level::Level()
	:tilePix(0), tileCountX(0), tileCountY(0), td(), whichEntity(0)
{}
const char* Level::getLevelName(){ return (td.fname).c_str(); }

Level::Level(const char* flareName, const char* mapName,
	int tilePix, int tileCountX, int tileCountY)
	: tilePix(tilePix), tileCountX(tileCountX), tileCountY(tileCountY), td(LoadTextureRGB(mapName)),
	whichEntity(0) { load(flareName); }

Level::Level(const char* flareName, TextureData td,
	int tilePix, int tileCountX, int tileCountY)
	: tilePix(tilePix), tileCountX(tileCountX), tileCountY(tileCountY), td(td),
	whichEntity(0) { load(flareName); }

void Level::load(const char* flareName){
	std::ifstream infile(flareName);
	std::string line;
	while (getline(infile, line)) {
		if (line == "[header]") { loadHeader(infile); }
		else if (line == "[layer]") { loadLevel(infile); }
		else if (line == "[StartLocations]") { loadStarts(infile); }
	}
	infile.close();
}

void Level::loadHeader(std::ifstream& infile){
	std::string line;

	while (getline(infile, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") { width = atoi(value.c_str()); }
		else if (key == "height"){ height = atoi(value.c_str()); }
	}
	OutputDebugString((std::to_string(width) + ' ' + std::to_string(height)).c_str());
	
	offsetX = -TILEUNITS * width / 2;
	offsetY = TILEUNITS * height / 2;
	OutputDebugString((std::to_string(offsetX) + ' ' + std::to_string(offsetY)).c_str());

	data.clear();
	for (int i = 0; i < height; i++) { data.push_back(std::vector<int>()); }
	OutputDebugString(std::to_string(data.size()).c_str());
}

void Level::loadLevel(std::ifstream& infile){
	OutputDebugString("gonna loadLevel");
	tileVerts.clear(); tileTexts.clear(); indices.clear();
	std::string line;
	while (getline(infile, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int row = 0; row < height; row++) {
				getline(infile, line);
				std::istringstream lineStream(line);
				std::string tile;
				for (int col = 0; col < width; col++) {
					getline(lineStream, tile, ',');
					int val = atoi(tile.c_str());
					data[row].push_back(val-1);
				}
			}
		}
	}
	OutputDebugString("loadedLevel");
	fillRenderVectors();
}

void Level::fillRenderVectors(){
	OutputDebugString("gonna fillRenderVectors");
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (!data[y][x]) { continue; }

			float u = (float)(data[y][x] % tileCountX) / (float)tileCountX;
			float v = (float)(data[y][x] / tileCountX) / (float)tileCountY;

			tileVerts.insert(tileVerts.end(), {
				TILEUNITS * x, -TILEUNITS * y,
				TILEUNITS * x, (-TILEUNITS * y) - TILEUNITS,
				(TILEUNITS * x) + TILEUNITS, (-TILEUNITS * y) - TILEUNITS,
				(TILEUNITS * x) + TILEUNITS, -TILEUNITS * y
			});

			float spriteWidth = 1.0f / (float)tileCountX;
			float spriteHeight = 1.0f / (float)tileCountY;
			tileTexts.insert(tileTexts.end(), { u, v,
				u, v + (spriteHeight),
				u + spriteWidth, v + (spriteHeight),
				u + spriteWidth, v
			});
		}
	}
	unsigned int blah[] = { 0, 1, 2, 0, 2, 3 };
	for (size_t i = 0; i < tileVerts.size(); i += 8){
		for (int j = 0; j < 6; j++){
			indices.push_back(blah[j] + i / 2);
		}
	}
	OutputDebugString("filledRenderVectors");
}

bool Level::solidTile(float x, float y){
	int tileCol, tileRow;
	world2tile(x, y, &tileCol, &tileRow);

	int t = (data[tileRow])[tileCol];

	return isSolid(t, (td.fname).c_str());	
}

float Level::tileCollide(float x, float y, float v, float h, bool isY){
	if (solidTile(x,y)) {
		int tileCol, tileRow;
		world2tile(x, y, &tileCol, &tileRow);

		float tileX, tileY;
		tile2world(&tileX, &tileY, tileCol, tileRow);
		return depenetrate(v, h, isY ? tileY : tileX, TILEUNITS / 2);
	}
	return v;
}

void Level::tile2world(float* worldX, float* worldY, int tileCol, int tileRow){
	//Center of tile
	*worldX = TILEUNITS*tileCol + offsetX + TILEUNITS / 2;
	*worldY = -TILEUNITS*tileRow + offsetY - TILEUNITS / 2;
}
void Level::world2tile(float worldX, float worldY, int* tileCol, int* tileRow){
	*tileCol = (int) ((worldX - offsetX) / TILEUNITS);
	*tileRow = (int)((worldY - offsetY) / -TILEUNITS);
}

void Level::draw(){
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(offsetX, offsetY, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, td.id);
	glVertexPointer(2, GL_FLOAT, 0, tileVerts.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, tileTexts.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void Level::loadStarts(std::ifstream& infile){
	//OutputDebugString("gonna loadStarts");
	std::string line, name, typeName;
	WhereToStart wts;
	while (getline(infile, line)) {
		if (line == "") { break; }
		if (line[0] == '#') { name = line.substr(2, line.size());  continue; }
		std::istringstream sStream(line);
		std::string key, value, buf;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") { typeName = value; }
		else if (key == "location"){
			wts.name = name; wts.typeName = typeName;

			std::istringstream lineStream(value);
			getline(lineStream, buf, ',');		int col = atoi(buf.c_str());
			getline(lineStream, buf, ',');		int row = atoi(buf.c_str());
			wts.col = col; wts.row = row;

			tile2world(&(wts.x), &(wts.y), col, row);

			startLocs.push_back(wts);
		}
	}
	//OutputDebugString("loadedStarts");
}

const WhereToStart* Level::getNext(){
	if (whichEntity < startLocs.size()){ return &(startLocs[whichEntity++]); }
	whichEntity = 0;
	return NULL;
}