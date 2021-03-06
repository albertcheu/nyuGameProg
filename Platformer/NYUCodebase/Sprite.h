#pragma once
#include "Helpers.h"

typedef struct{
	//Bundle that holds id, pixel width, pixel height
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
	float u, v, w, h;
} SpriteFrame;

class AnimCycle{
private:
	int swidth, sheight, width, height, numFrames, row, col, i;
public:
	AnimCycle(int swidth, int sheight, int width, int height,
		int numFrames, int row, int col=0);
	SpriteFrame getNext();
};