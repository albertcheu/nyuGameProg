#pragma once
#include "theSDLheaders.h"
#include "Sprite.h"

class Entity{
private:
	Sprite s;
	float x, y, width, height, angle, xSpeed, ySpeed, halfWidth, halfHeight;
	//Note that "angle" means angle of drawing, not angle of motion

	bool visible;

	void calcHalves();

public:
	Entity();
	Entity(float x, float y, float width, float height, Sprite s, bool visible=true);

	void draw();

	void setX(float xPos);
	void setY(float yPos);
	void setPos(float xPos, float yPos);
	void setAngle(float theta);
	void setSize(float scale);
	void setSize(float newWidth, float newHeight);

	void setVisibility(bool v);

	void setSpeed(float v, float dir);
	void setXspeed(float val);
	void setYspeed(float val);

	void reset();

	float getX();
	float getY();
	float getWidth();
	float getHeight();
	float getAngle();
	float getXspeed();
	float getYspeed();
	float getHalfWidth();
	float getHalfHeight();
	bool getVisibility();

	bool collide(const Entity& other);
};