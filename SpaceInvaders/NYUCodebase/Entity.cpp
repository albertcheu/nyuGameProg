#include "Entity.h"
void Entity::calcHalves(){
	halfWidth = width / 2.0f;
	halfHeight = height / 2.0f;
}

Entity::Entity()
	:x(0), y(0), width(0), height(0), angle(0),
	xSpeed(0), ySpeed(0), halfWidth(0), halfHeight(0),
	visible(true), s()
{}

Entity::Entity(float x, float y, float width, float height, Sprite s, bool visible)
	:x(x), y(y), width(width), height(height), s(s),
	angle(0.0f), xSpeed(0.0f), ySpeed(0.0f), visible(visible)
{
	calcHalves();
	
}

void Entity::setVisibility(bool v){ visible = v; }
bool Entity::getVisibility() { return visible; }

void Entity::setUV(float newU, float newV){ s.setUV(newU,newV); }

void Entity::draw(){
	if (visible){
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(x, y, 0.0f);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		s.draw(width, height);
	}
}

void Entity::setSize(float scale){
	width *= scale;
	height *= scale;
	calcHalves();
}
void Entity::setSize(float newWidth, float newHeight){
	width = newWidth;
	height = newHeight;
	calcHalves();
}

void Entity::setX(float xPos) { x = xPos; }
void Entity::setY(float yPos) { y = yPos; }
void Entity::setPos(float xPos, float yPos){
		x = xPos;
		y = yPos;
	}
void Entity::setPos(float elapsed){
	setX(x + elapsed*xSpeed);
	setY(y + elapsed*ySpeed);
}
void Entity::setAngle(float theta) { angle = theta; }
void Entity::setSpeed(float v, float dir) {
		xSpeed = v*cos(dir * M_PI / 180.0f);
		ySpeed = v*sin(dir * M_PI / 180.0f);
	}
void Entity::setXspeed(float val){ xSpeed = val; }
void Entity::setYspeed(float val){ ySpeed = val; }

void Entity::reset(){
	setYspeed(0);
	setXspeed(0);
	setPos(0, 0);
	setVisibility(false);
}

float Entity::getX(){ return x; }
float Entity::getY(){ return y; }
float Entity::getWidth(){ return width; }
float Entity::getHeight(){ return height; }
float Entity::getAngle(){ return angle; }
float Entity::getXspeed(){ return xSpeed; }
float Entity::getYspeed(){ return ySpeed; }
float Entity::getHalfWidth() { return halfWidth; }
float Entity::getHalfHeight() { return halfHeight; }

bool Entity::collide(const Entity& other){
	//Are our boxes overlapping?
	return !(
		(x + halfWidth < other.x - other.halfWidth) ||
		(other.x + other.halfWidth < x - halfWidth) ||
		(y + halfHeight < other.y - other.halfHeight) ||
		(other.y + other.halfHeight < y - halfHeight)
		);
}