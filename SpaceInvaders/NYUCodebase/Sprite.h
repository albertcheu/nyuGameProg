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
	
	//Draw the sprite to fit w units wide and h units tall
	void draw(float w, float h);
	void setUV(float newU, float newV);
};