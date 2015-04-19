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
{
	calcHalves();
	buildTransform();
}

void Entity::setVisibility(bool v){ visible = v; }
bool Entity::getVisibility() { return visible; }

void Entity::setUV(float newU, float newV){ s.setUV(newU,newV); }

void Entity::buildTransform(){
	Matrix rot, transl;
	rot.m[0][0] = rot.m[1][1] = cos(angle*M_PI / 180.0f);
	rot.m[0][1] = sin(angle*M_PI / 180.0f);
	rot.m[1][0] = -1 * rot.m[0][1];
	transl.m[3][0] = x;
	transl.m[3][1] = y;
	mat = transl*rot;
}

void Entity::draw(){
	if (visible){
		buildTransform();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glMultMatrixf(mat.ml);
		s.draw(width, height);

		glPopMatrix();
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

void Entity::setX(float xPos) { x = xPos; } void Entity::setY(float yPos) { y = yPos; }
void Entity::setPos(float xPos, float yPos){ x = xPos; y = yPos; }
void Entity::setAngle(float theta) { angle = theta; }

float Entity::getX(){ return x; } float Entity::getY(){ return y; }
float Entity::getWidth(){ return width; } float Entity::getHeight(){ return height; }
float Entity::getAngle(){ return angle; }

float Entity::getHalfWidth() { return halfWidth; }
float Entity::getHalfHeight() { return halfHeight; }

Vector Entity::pushOut(Entity& other){
	buildTransform();	other.buildTransform();

	return sat(mat, halfWidth, halfHeight,
		other.mat, other.halfWidth, other.halfHeight);
}
bool Entity::collide(Entity& other){
	Vector v = pushOut(other);
	
	if (x < other.x) { x -= fabs(v.x); }
	else if (x > other.x) { x += fabs(v.x); }

	if (y < other.y) { y -= fabs(v.y); }
	else if (y > other.y) { y += fabs(v.y); }

	return !(v.x == 0 && v.y == 0);
}

bool Dynamic::collide(Entity& other){
	float oldX = x; float oldY = y;
	bool ans = Entity::collide(other);
	if (oldX < x) { stickLeft(x); }
	else if (oldX > x) { stickRight(x); }
	if (oldY < y) { stickBottom(y); }
	else if (oldY > y) { stickTop(y); }
	return ans;
}

bool Dynamic::collideBounce(Dynamic& enemy, float bounceMag){
	//Check how much we need to depenetrate
	Vector v = pushOut(enemy);
	//Collide with it
	if (v.x == 0 && v.y == 0) { return false; }

	//Enemy is to our right and nothing to our left
	if (x < enemy.x && !touchLeft) { vx = -bounceMag; }
	//Enemy is to our left and nothing to our right
	else if (enemy.x < x && !touchRight){ vx = bounceMag; }

	//Enemy above and nothing below
	if (y < enemy.y && !touchBottom) { vy = -bounceMag; }
	//Enemy below and nothing above
	else if (enemy.y < y && !touchTop) { vy = bounceMag; }

	return true;
}

ColoredDir::ColoredDir():Entity(),color(RED),dir(BEAMDIR_LEFT){}
ColoredDir::ColoredDir(float x, float y, float width, float height,
	Sprite s, BeamColor color, int dir): Entity(x, y, width, height, s), color(color), dir(dir){}
BeamColor ColoredDir::getColor(){ return color; }
int ColoredDir::getDir(){ return dir; }

Beam::Beam() : ColoredDir(), soundPtr(NULL) { visible = false; }
Beam::Beam(float width, float height, Sprite s, BeamColor color, Mix_Chunk* sp)
	: ColoredDir(0, 0, width, height, s, color, dir), soundPtr(sp) { visible = false; }

void Beam::fire(float xcoor, float ycoor, int newDir){
	visible = true; x = xcoor; y = ycoor; dir = newDir;
	angle = (dir == BEAMDIR_LEFT ? 180.0f: 0);
	Mix_PlayChannel(-1, soundPtr, 0);
}
bool Beam::hit(Door& d){
	if (this->collide(d) && d.getVisibility()){
		visible = false;
		if (color == d.getColor()) { d.open(); }
		return true;
	}
	return false;
}
void Beam::freeSound(){ Mix_FreeChunk(soundPtr); }

Door::Door() : ColoredDir(), move(false), complement(NULL){}
Door::Door(float x, float y, Sprite s, BeamColor color, int dir)
	: ColoredDir(x, y, TILEUNITS, TILEUNITS * 4, s, color, dir),
	complement(NULL), move(false){}
void Door::setComplement(Door* d){ complement = d; }
void Door::open(){ move = true; complement->move = true; }
void Door::disappear(){
	if (this->collide(*complement)){
		visible = false; complement->visible = false;
		move = false; complement->move = false;
	}
}
bool Door::moving(){ return move; }

Pickup::Pickup() :Entity(){ visible = false; }
Pickup::Pickup(TextureData td, float u_offset)
	: Entity(0, 0, TILEUNITS, TILEUNITS,
				Sprite(td.id, u_offset*TILEPIX / td.width,
				4.0f*TILEPIX / td.height,
				1.0f*TILEPIX / td.width,
				1.0f*TILEPIX / td.height),
			false), acquired(false){}
void Pickup::activate(float newX, float newY){ x = newX; y = newY; visible = true; }
void Pickup::hit(Entity* player, Mix_Chunk* soundPtr){
	if (visible && this->collide(*player)){
		Mix_PlayChannel(-1, soundPtr, 0);
		acquired = true; visible = false;		
	}
}
bool Pickup::have(){ return acquired; }

Dynamic::Dynamic()
	: Entity(), vx(0), vy(0), ax(0), ay(0),
	touchTop(false), touchLeft(false), touchBottom(false), touchRight(false), et(NOT_ENEMY)
{}

Dynamic::Dynamic(float x, float y, float width, float height, Sprite s, EnemyType et)
	: Entity(x, y, width, height, s), vx(0), vy(0), ax(0), ay(0),
	touchTop(false), touchLeft(false), touchBottom(false), touchRight(false), et(et)
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

EnemyType Dynamic::getType(){ return et; }

AnimatedDynamic::AnimatedDynamic(){}
AnimatedDynamic::AnimatedDynamic(float x, float y, float width, float height,
	Sprite s, EnemyType et)
	:Dynamic(x,y,width,height,s,et)
{}
void AnimatedDynamic::setCycle(AnimCycle& ac){ cycle = ac; }
void AnimatedDynamic::nextFrame(){
	setFrame(cycle.getNext());
}