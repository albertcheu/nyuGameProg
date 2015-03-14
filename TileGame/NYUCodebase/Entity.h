#pragma once
#include "Sprite.h"

class Entity{
protected:
	Sprite s;
	float x, y, width, height, angle, halfWidth, halfHeight;
	//Note that "angle" means angle of drawing, not angle of motion

	bool visible;

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

	void setVisibility(bool v); bool getVisibility();
	
	float getX();	float getY();
	float getWidth();	float getHeight();
	float getAngle();

	float getHalfWidth();	float getHalfHeight();	

	bool collide(const Entity& other);
};

class Dynamic: public Entity{
private:
	float vx, vy, ax, ay;
	bool touchTop, touchLeft, touchBottom, touchRight;
public:

	Dynamic();
	Dynamic(float x, float y, float width, float height, Sprite s, bool visible=true);
	float getAx();	float getAy();
	float getVx();	float getVy();
	
	void reset();
	void setLeft(); void setRight(); void setTop(); void setBottom();
	bool getLeft(); bool getRight(); bool getTop(); bool getBottom();
	void setSpeed(float v, float dir);	void setVx(float val);	void setVy(float val);
	void bumpX(float val); void bumpY(float val); void bumpVx(float val); void bumpVy(float val);
	void setAx(float val); void setAy(float val);
	void noTouch();

	void stickLeft(float val); void stickRight(float val);
	void stickTop(float val); void stickBottom(float val);

	void setFrame(SpriteFrame sf);
};