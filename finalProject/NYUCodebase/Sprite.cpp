#include "Sprite.h"

TextureData LoadTexture(const char *image_path, int internalFormat, GLenum format){
	SDL_Surface *surface = IMG_Load(image_path);
	if (!surface) { OutputDebugString("Error loading image"); }

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//OutputDebugString(std::to_string(textureID).c_str());

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
		surface->w, surface->h, 0, format,
		GL_UNSIGNED_BYTE, surface->pixels);
	//OutputDebugString("Loaded surface");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//OutputDebugString("Texparemteri");

	TextureData ret = { image_path, textureID, surface->w, surface->h };
	SDL_FreeSurface(surface);
	//OutputDebugString("Freed surface");

	return ret;
}

TextureData LoadTextureRGB(const char *image_path)
	{ return LoadTexture(image_path, GL_RGB, GL_RGB); }
TextureData LoadTextureRGBA(const char *image_path)
	{ return LoadTexture(image_path, GL_RGBA, GL_RGBA); }

Sprite::Sprite()
	:textureID(0), u(1.0f), v(1.0f), width(1.0f), height(1.0f)
{}
Sprite::Sprite(GLuint textureID, float u, float v, float width, float height)
	:textureID(textureID), u(u), v(v), width(width), height(height)
{}

void Sprite::draw(float presentationWidth, float presentationHeight) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	float hw = presentationWidth / 2.0f;
	float hh = presentationHeight / 2.0f;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	GLfloat pts[] = { -hw, hh, -hw, -hh, hw, -hh, hw, hh };
	glVertexPointer(2, GL_FLOAT, 0, pts);
	glEnableClientState(GL_VERTEX_ARRAY);
	GLfloat uvs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, uvs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

	glDisable(GL_TEXTURE_2D);
}

void Sprite::setUV(float newU, float newV){
	u = newU;
	v = newV;
}
void Sprite::setSize(float newWidth, float newHeight){ width = newWidth; height = newHeight; }

Text::Text(){}
Text::Text(GLuint textureID, float charSizeSheet, float charSize, float x, float y, std::string s)
	:textureID(textureID), charSizeSheet(charSizeSheet), charSize(charSize), x(x), y(y)
{
	changeText(s);
}

void Text::changeText(std::string s){
	points.clear(); uvs.clear(); indices.clear();
	int numChars = s.size();
	float vertexX = x + ((float)numChars * charSize) / -2.0f;
	float vertexY = y + charSize / 2.0f;

	for (size_t i = 0; i < s.size(); i++){
		int val = (int)(s[i]);
		int col = val % 16; int row = val / 16;
		add(row, col, vertexX, vertexY);
		vertexX += charSize;
	}

	unsigned int blah[] = { 0, 1, 2, 0, 2, 3 };
	for (size_t i = 0; i < points.size(); i += 8){
		for (int j = 0; j < 6; j++){
			indices.push_back(blah[j] + i / 2);
		}
	}
}

void Text::add(int row, int col, float vertexX, float vertexY){
	points.push_back(vertexX); points.push_back(vertexY);
	points.push_back(vertexX); points.push_back(vertexY - charSize);
	points.push_back(vertexX + charSize); points.push_back(vertexY - charSize);
	points.push_back(vertexX + charSize); points.push_back(vertexY);
	float v = row*charSizeSheet;	float u = col *charSizeSheet;
	uvs.push_back(u); uvs.push_back(v);
	uvs.push_back(u); uvs.push_back(v + charSizeSheet);
	uvs.push_back(u + charSizeSheet); uvs.push_back(v + charSizeSheet);
	uvs.push_back(u + charSizeSheet); uvs.push_back(v);
}

void Text::draw(){
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexPointer(2, GL_FLOAT, 0, points.data());
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, uvs.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisable(GL_TEXTURE_2D);

}

AnimCycle::AnimCycle(int numFrames, float cu, float cv, int dir, float width, float height) :i(0){
	for (int j = 0; j < numFrames; j++){
		sfs.push_back(SpriteFrame());
		int offset = (dir < 0) ? (numFrames - j) : (numFrames - j - 1);
		sfs.back().u = cu + dir*offset*width; sfs.back().v = cv;
		sfs.back().w = width; sfs.back().h = height;
	}	
}
void AnimCycle::setFrame(size_t index, float u, float width){
	sfs[index].u = u;
	sfs[index].w = width;
}
void AnimCycle::merge(AnimCycle& other){
	for (size_t j = 0; j < other.sfs.size(); j++){
		sfs.push_back(other.sfs[j]);
	}
}

void AnimCycle::reorder(size_t s, const size_t* newOrder){
	std::vector<SpriteFrame> newSfs;
	for (size_t j = 0; j < s; j++){ newSfs.push_back(sfs[newOrder[j]]);	}
	sfs = newSfs;
}

SpriteFrame AnimCycle::getNext(){
	size_t index = i++;
	if (i == sfs.size()) { i = 0; }
	return sfs[index];
}