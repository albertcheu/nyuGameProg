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
	
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	GLfloat pts[] = { -hw, hh, -hw, -hh, hw, -hh, hw, hh };
	glVertexPointer(2, GL_FLOAT, 0, pts);
	glEnableClientState(GL_VERTEX_ARRAY);
	GLfloat uvs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, uvs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

	glDisable(GL_TEXTURE_2D);
}

void Sprite::setUV(float newU, float newV){
	u = newU;
	v = newV;
}
void Sprite::setSize(float newWidth, float newHeight){ width = newWidth; height = newHeight; }

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