#include "Sprite.h"

TextureData LoadTexture(const char *image_path, int internalFormat, GLenum format){
	SDL_Surface *surface = IMG_Load(image_path);
	if (!surface) { OutputDebugString("Error loading image"); }

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
		surface->w, surface->h, 0, format,
		GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	TextureData ret = { image_path, textureID, surface->w, surface->h };
	SDL_FreeSurface(surface);
	
	return ret;
}

TextureData LoadTextureRGB(const char *image_path){ return LoadTexture(image_path, GL_RGB, GL_RGB); }
TextureData LoadTextureRGBA(const char *image_path){ return LoadTexture(image_path, GL_RGBA, GL_RGBA); }

Sprite::Sprite()
	:textureID(0), u(1.0f), v(1.0f), width(1.0f), height(1.0f)
{}
Sprite::Sprite(GLuint textureID, float u, float v, float width, float height)
	:textureID(textureID), u(u), v(v), width(width), height(height)
{}

void Sprite::draw(float w, float h) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	w /= 2.0f;
	h /= 2.0f;

	GLfloat quad[] = { -w, h, -w, -h, w, -h, w, h};
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisable(GL_TEXTURE_2D);
}

void Sprite::setUV(float newU, float newV){
	u = newU;
	v = newV;
}
void Sprite::setSize(float newWidth, float newHeight){ width = newWidth; height = newHeight; }

AnimCycle::AnimCycle(int swidth, int sheight, int width, int height,
	int numFrames, int row, int col)
	:swidth(swidth), sheight(sheight), width(width), height(height),
	numFrames(numFrames), row(row), col(col), i(col)
{}
SpriteFrame AnimCycle::getNext(){
	float u = (64 * i + ((64.0f - width) / 2)) / swidth;
	float v = ((64.0f * row) - height) / sheight;
	SpriteFrame ans = { u, v, 1.0f*width/swidth, 1.0f*height/sheight };
	i++;
	if (i - col == numFrames){ i = col; }
	return ans;
}