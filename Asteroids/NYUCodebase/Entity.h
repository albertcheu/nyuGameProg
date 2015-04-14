#pragma once
#include "Sprite.h"
#include "Matrix.h"

#define BEAMDIR_LEFT -1
#define BEAMDIR_RIGHT 1

class Entity{
protected:
	Matrix mat;
	Sprite s;

	//Assume for now this is a 2D game without scaling
	float x, y, width, height, angle, halfWidth, halfHeight;
	//Note that "angle" is not necessarily angle of motion

	bool visible;

	void calcHalves();

public:
	Entity();
	Entity(float x, float y, float width, float height, Sprite s, bool visible=true);
	
	void buildTransform();
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

	bool collide(Entity& other);
};

enum BeamColor{RED,YELLOW,GREEN,BLUE};

//This derivative of Entity contains color and direction info
//Beam and Door are derived in turn
class ColoredDir:public Entity{
protected:
	BeamColor color;
	int dir;
public:
	ColoredDir();
	ColoredDir(float x, float y, float width, float height, Sprite s, BeamColor color, int dir);
	BeamColor getColor();
	int getDir();
};

class Beam: public ColoredDir{
private:
	Mix_Chunk* soundPtr;
public:
	Beam();
	Beam(float width, float height, Sprite s, BeamColor color, Mix_Chunk* sp);
	//Set location, direction, and become visible (active)
	void fire(float x, float y, int newDir);

	void moveByAngle(float speed, float timestep);
};

class Pickup:public Entity{
private:
	//Have we acquired this pickup
	bool acquired;
public:
	//Pickups start out at the origin and have dimensions TILEUNITS*TILEUNITS
	Pickup();
	Pickup(TextureData td, float u_offset);
	//Make the pickup availble for pickup at the specified location
	void activate(float newX, float newY);
	//If the player collided with this pickup, turn invisible and make a sound
	void hit(Entity* player, Mix_Chunk* soundPtr);
	//Did the player collect this?
	bool have();
};

enum EnemyType{NOT_ENEMY,HOPPER,RUNNER};
class Dynamic: public Entity{
private:
	float vx, vy, ax, ay;
	bool touchTop, touchLeft, touchBottom, touchRight;
	EnemyType et;

public:
	Dynamic();
	Dynamic(float x, float y, float width, float height, Sprite s, EnemyType et);
	float getAx();	float getAy();
	float getVx();	float getVy();
	
	void reset();
	
	bool getLeft(); bool getRight(); bool getTop(); bool getBottom();

	void setSpeed(float v, float dir);	void setVx(float val);	void setVy(float val);
	
	void setAx(float val); void setAy(float val);
	void moveY(float timestep, float friction, float gravity);
	void moveX(float timestep, float friction);
	void noTouch();

	void stickLeft(float val); void stickRight(float val);
	void stickTop(float val); void stickBottom(float val);

	void setFrame(SpriteFrame sf);

	EnemyType getType();

	bool collide(Entity& other);
};