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

typedef struct{
	//Location and size of the sprite in the sheet, normalized
	float u, v, w, h;
} SpriteFrame;

class Sprite {
protected:
	//Id of the sheet
	GLuint textureID;

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

enum AnimMode{ RESET, PINGPONG, STOP };
class AnimCycle{
private:
	std::vector<SpriteFrame> sfs; size_t i;
	int sign;
	AnimMode am;
public:
	AnimCycle();
	AnimCycle(int numFrames, float cu, float cv, int dir, float width, float height);
	void changeMode(AnimMode newMode);
	SpriteFrame getNext();

	void setFrame(size_t index, float u, float width);
	void merge(AnimCycle& other);
	void reorder(size_t s, const size_t* newOrder);
};

class AnimatedSprite: public Sprite{
public:
	AnimatedSprite();
	AnimatedSprite(GLuint textureID, float u, float v, float width, float height,
		AnimCycle cycle, float threshold, float currentTime);

	//If the time passed since the lastChange is greater than our threshold,
	//change the sprite's uv-coordinates and tell the calling function we did so
	bool draw(float presentationWidth, float presentationHeight, float currentTime);
private:
	AnimCycle cycle;
	float threshold, lastChange;
};