#pragma once
#include "Sprite.h"

class Entity{
private:
	Sprite s;
	float x, y, width, height, angle, vx, vy, ax, ay, halfWidth, halfHeight;
	//Note that "angle" means angle of drawing, not angle of motion

	bool visible, touchTop, touchLeft, touchBottom, touchRight;

	void calcHalves();

public:
	Entity();
	Entity(float x, float y, float width, float height, Sprite s, bool visible=true);

	void setUV(float newU, float newV);
	void draw();

	void setX(float xPos);	void setY(float yPos);
	void setPos(float xPos, float yPos);
	void setAngle(float theta);
	void setSize(float scale);	void setSize(float newWidth, float newHeight);

	void setVisibility(bool v);
	void setLeft(); void setRight(); void setTop(); void setBottom();
	bool getLeft(); bool getRight(); bool getTop(); bool getBottom();
	void setSpeed(float v, float dir);	void setVx(float val);	void setVy(float val);
	void bumpX(float val); void bumpY(float val); void bumpVx(float val); void bumpVy(float val);
	void setAx(float val); void setAy(float val);

	void noTouch();
	void reset();

	float getX();	float getY();
	float getWidth();	float getHeight();
	float getAngle();
	float getAx();	float getAy();
	float getVx();	float getVy();
	float getHalfWidth();	float getHalfHeight();

	bool getVisibility();

	bool collide(const Entity& other);
};