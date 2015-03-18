#include "Entity.h"
void Entity::calcHalves(){
	halfWidth = width / 2.0f;
	halfHeight = height / 2.0f;
}

Entity::Entity()
	:x(0), y(0), width(0.01f), height(0.01f), angle(0),
	halfWidth(0), halfHeight(0), s(),
	visible(true)
{}

Entity::Entity(float x, float y, float width, float height, Sprite s, bool visible)
	:x(x), y(y), width(width), height(height), s(s), angle(0), visible(visible)
{ calcHalves(); }

void Entity::setVisibility(bool v){ visible = v; }
bool Entity::getVisibility() { return visible; }

void Entity::setUV(float newU, float newV){ s.setUV(newU,newV); }

void Entity::draw(float delX, float delY){
	if (visible){
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(delX, delY, 0);

		glTranslatef(x, y, 0.0f);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		s.draw(width, height);

		glPopMatrix();
	}
}

void Entity::draw(){ draw(0, 0); }

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

void Entity::setX(float xPos) { x = xPos; } void Entity::setY(float yPos) { y = yPos; }
void Entity::setPos(float xPos, float yPos){ x = xPos; y = yPos; }
void Entity::setAngle(float theta) { angle = theta; }

float Entity::getX(){ return x; } float Entity::getY(){ return y; }
float Entity::getWidth(){ return width; } float Entity::getHeight(){ return height; }
float Entity::getAngle(){ return angle; }

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

Beam::Beam() :Entity(), color(RED) { visible = false; }
Beam::Beam(float width, float height, Sprite s, int color)
	: Entity(0, 0, width, height, s, false), color(color)
{}

void Beam::fire(float xcoor, float ycoor, float dir){
	visible = true; x = xcoor; y = ycoor; angle = dir;
}

Door::Door() : Entity(), color(RED), dir(0), move(false), complement(NULL){}
Door::Door(float x, float y, Sprite s, int color, int dir)
	: Entity(x, y, TILEUNITS, TILEUNITS * 4, s),
	color(color), dir(dir), complement(NULL), move(false)
{}
void Door::setComplement(Door* d){ complement = d; }
void Door::hit(int beamColor){
	if (beamColor == color){ move = true; complement->move = true; }
}
void Door::disappear(){
	if (this->collide(*complement)){
		visible = false; complement->visible = false;
		move = false; complement->move = false;
	}
}
bool Door::moving(){ return move; }

Dynamic::Dynamic()
	: Entity(), vx(0), vy(0), ax(0), ay(0),
	touchTop(false), touchLeft(false), touchBottom(false), touchRight(false)
{}

Dynamic::Dynamic(float x, float y, float width, float height, Sprite s, bool visible)
	: Entity(x, y, width, height, s, visible), vx(0), vy(0), ax(0), ay(0),
	touchTop(false), touchLeft(false), touchBottom(false), touchRight(false)
{}

void Dynamic::setSpeed(float v, float dir) {
	vx = v*cos(dir * M_PI / 180.0f);
	vy = v*sin(dir * M_PI / 180.0f);
}
void Dynamic::setVx(float val){ vx = val; } void Dynamic::setVy(float val){ vy = val; }
void Dynamic::setAx(float val){ ax = val; } void Dynamic::setAy(float val){ ay = val; }
void Dynamic::noTouch(){ touchBottom = touchTop = touchRight = touchLeft = false; }

void Dynamic::reset(){
	setAy(0);	setAx(0);	setVy(0);	setVx(0);	setPos(0, 0);
	setVisibility(false); noTouch();
}

float Dynamic::getVx(){ return vx; } float Dynamic::getVy(){ return vy; }
float Dynamic::getAx(){ return ax; } float Dynamic::getAy(){ return ay; }
bool Dynamic::getLeft(){ return touchLeft; } bool Dynamic::getRight(){ return touchRight; }
bool Dynamic::getTop(){ return touchTop; } bool Dynamic::getBottom(){ return touchBottom; }

void Dynamic::stickLeft(float val){ touchLeft = true; vx = 0; x = val; }
void Dynamic::stickRight(float val){ touchRight = true; vx = 0; x = val; }
void Dynamic::stickTop(float val){ touchTop = true; vy = 0; y = val; }
void Dynamic::stickBottom(float val){ touchBottom = true; vy = 0; y = val; }

void Dynamic::moveY(float timestep, float friction, float gravity){
	vy = lerp(vy, 0, timestep*friction);
	vy += ay * timestep + gravity*timestep;
	y += vy*timestep;
}

void Dynamic::moveX(float timestep, float friction){
	vx = lerp(vx, 0, timestep*friction);
	vx += ax * timestep;
	x += vx*timestep;
}

void Dynamic::setFrame(SpriteFrame sf){
	s.setUV(sf.u, sf.v); s.setSize(sf.w, sf.h);
}