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

	void draw(float presentationWidth, float presentationHeight);

};

class Text{
private:
	GLuint textureID;
	float charSizeSheet, charSize, x, y;
	std::vector<float> points, uvs;
	std::vector<unsigned int> indices;
	void add(int row, int col, float vertexX, float vertexY);

public:
	Text();
	Text(GLuint textureID, float charSizeSheet, float charSize, float x, float y, std::string s);
	void changeText(std::string s);
	void draw();
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