#include "Entity.h"
void Entity::calcHalves(){
	halfWidth = width / 2.0f;
	halfHeight = height / 2.0f;
}

Entity::Entity()
	:x(0), y(0), width(0), height(0), angle(0),
	vx(0), vy(0), ax(0), ay(0), halfWidth(0), halfHeight(0),
	visible(true), s(), touchTop(false), touchLeft(false), touchBottom(false), touchRight(false)
{}

Entity::Entity(float x, float y, float width, float height, Sprite s, bool visible)
	:x(x), y(y), width(width), height(height), s(s),
	angle(0), vx(0), vy(0), ax(0), ay(0),
	visible(visible), touchTop(false), touchLeft(false), touchBottom(false), touchRight(false)
{ calcHalves(); }

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

void Entity::bumpX(float val){ x += val; } void Entity::bumpY(float val){ y += val; }
void Entity::setX(float xPos) { x = xPos; } void Entity::setY(float yPos) { y = yPos; }
void Entity::setPos(float xPos, float yPos){ x = xPos; y = yPos; }
void Entity::setAngle(float theta) { angle = theta; }
void Entity::setSpeed(float v, float dir) {
	vx = v*cos(dir * M_PI / 180.0f);
	vy = v*sin(dir * M_PI / 180.0f);
}

void Entity::bumpVx(float val){ vx += val; } void Entity::bumpVy(float val){ vy += val; }
void Entity::setVx(float val){ vx = val; } void Entity::setVy(float val){ vy = val; }
void Entity::setAx(float val){ ax = val; } void Entity::setAy(float val){ ay = val; }
void Entity::setLeft(){ touchLeft = true; } void Entity::setRight(){ touchRight = true; }
void Entity::setTop(){ touchTop = true; } void Entity::setBottom(){ touchBottom = true; }
void Entity::noTouch(){ touchBottom = touchTop = touchRight = touchLeft = false; }

void Entity::reset(){
	setAy(0);	setAx(0);	setVy(0);	setVx(0);	setPos(0, 0);
	setVisibility(false); noTouch();
}

float Entity::getX(){ return x; } float Entity::getY(){ return y; }
float Entity::getWidth(){ return width; } float Entity::getHeight(){ return height; }
float Entity::getAngle(){ return angle; }
float Entity::getVx(){ return vx; } float Entity::getVy(){ return vy; }
float Entity::getAx(){ return ax; } float Entity::getAy(){ return ay; }
float Entity::getHalfWidth() { return halfWidth; }
float Entity::getHalfHeight() { return halfHeight; }
bool Entity::getLeft(){ return touchLeft; } bool Entity::getRight(){ return touchRight; }
bool Entity::getTop(){ return touchTop; } bool Entity::getBottom(){ return touchBottom; }

bool Entity::collide(const Entity& other){
	//Are our boxes overlapping?
	return !(
		(x + halfWidth < other.x - other.halfWidth) ||
		(other.x + other.halfWidth < x - halfWidth) ||
		(y + halfHeight < other.y - other.halfHeight) ||
		(other.y + other.halfHeight < y - halfHeight)
		);
}