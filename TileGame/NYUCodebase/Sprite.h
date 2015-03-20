#pragma once
#include "Helpers.h"

typedef struct{
	std::string fname;
	GLuint id;
	int width;
	int height;
} TextureData;

TextureData LoadTexture(const char *image_path, int internalFormat, GLenum format);

TextureData LoadTextureRGB(const char *image_path);
TextureData LoadTextureRGBA(const char *image_path);

class Sprite {
private:
	//Id of the sheet
	GLuint textureID;

	//Location and size of the sprite in the sheet, normalized
	float u, v, width, height;

public:
	Sprite();
	Sprite(GLuint textureID, float u, float v, float width, float height);
	
	void setUV(float newU, float newV);
	void setSize(float newWidth, float newHeight);

	//Draw the sprite to fit w units wide and h units tall
	void draw(float w, float h);

};

typedef struct{
	float u, v, w, h;//normalized
} SpriteFrame;

class AnimCycle{
private:
	std::vector<SpriteFrame> sfs; size_t i;
public:
	AnimCycle(int numFrames, float cu, float cv, int dir, float width, float height);
	void setFrame(size_t index, float u, float width);
	SpriteFrame getNext();
	void merge(AnimCycle& other);
	void reorder(size_t s, const size_t* newOrder);
};