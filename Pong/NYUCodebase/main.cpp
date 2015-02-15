#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BALL_SPEED 0.7f
#define LEFT 1
#define RIGHT -1
#define ANG_R 0.0f
#define ANG_L 180.0f
#define ANG_TR 45.0f
#define ANG_BR 315.0f
#define ANG_TL 135.0f
#define ANG_BL 225.0f

SDL_Window* displayWindow;

void setup(){
	//Boilerplate
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//Establish how big our screen is
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	//This is a 2D game, so it ought to be orthographic
	//Adjust the scale (ought to fit the screen!)
	glOrtho(-1.33, 1.33, -1, 1, -1, 1);
	
	//Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	OutputDebugString("Finished setup");
}

typedef struct{
	GLuint id;
	GLint width;
	GLint height;
} TextureData;

TextureData LoadTexture(const char *image_path, GLint internalFormat, GLenum format) {
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

	TextureData ret = { textureID, surface->w, surface->h };
	SDL_FreeSurface(surface);

	return ret;
}

TextureData LoadTextureRGB(const char *image_path){ return LoadTexture(image_path,GL_RGB,GL_RGB); }
TextureData LoadTextureRGBA(const char *image_path){ return LoadTexture(image_path, GL_RGBA, GL_RGBA); }

class Entity{
private:
	GLfloat x, y, width, height, angle, xSpeed, ySpeed, halfWidth, halfHeight;
	//Note that angle means angle of drawing, not angle of motion
	//the latter determines (and is determined) by xSpeed and ySpeed
	GLuint texId;
	void calcHalves(){
		halfWidth = width / 2.0f;
		halfHeight = height / 2.0f;
	}
public:
	Entity(const char *image_path, GLfloat xPos, GLfloat yPos, bool isRGBA=true)
	:x(xPos),y(yPos),angle(0.0f),xSpeed(0.0f),ySpeed(0.0f)
	{
		setTexture(image_path,isRGBA);
		calcHalves();
	}
	void setTexture(const char* image_path, bool isRGBA){
		TextureData td = isRGBA?LoadTextureRGBA(image_path):LoadTextureRGB(image_path);
		texId = td.id;
		//Assume for the moment that 300 pixels maps to one unit (i.e. no scale change)
		width = td.width / 300.0f;
		height = td.height / 300.0f;
	}
	void setTexture(GLuint id){ texId = id; }

	void setSize(GLfloat scale){
		width *= scale;
		height *= scale;
		calcHalves();
	}
	void setSize(GLfloat newWidth, GLfloat newHeight){
		width = newWidth;
		height = newHeight;
		calcHalves();
	}
	void setX(GLfloat xPos) { x = xPos; }
	void setY(GLfloat yPos) { y = yPos; }
	void setPos(GLfloat xPos, GLfloat yPos){
		x = xPos;
		y = yPos;
	}
	void setAngle(GLfloat theta) { angle = theta; }
	void setSpeed(GLfloat v, GLfloat dir) { 
		xSpeed = v*cos(dir * M_PI / 180.0f);
		ySpeed = v*sin(dir * M_PI / 180.0f);
	}
	void setXspeed(GLfloat val){ xSpeed = val; }
	void setYspeed(GLfloat val){ ySpeed = val; }

	GLfloat getX(){ return x; }
	GLfloat getY(){ return y; }
	GLuint getId(){ return texId; }
	GLfloat getWidth(){ return width; }
	GLfloat getHeight(){ return height; }
	GLfloat getAngle(){ return angle; }
	GLfloat getXspeed(){ return xSpeed; }
	GLfloat getYspeed(){ return ySpeed; }
	GLfloat getHalfWidth() { return halfWidth; }
	GLfloat getHalfHeight() { return halfHeight; }
};

void DrawEntity(Entity e) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, e.getId());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(e.getX(), e.getY(), 0.0);
	glRotatef(e.getAngle(), 0.0, 0.0, 1.0);

	GLfloat left, right, top, bottom;
	right = e.getHalfWidth();
	left = -right;
	top = e.getHalfHeight();
	bottom = -top;

	GLfloat quad[] = { left, top, left, bottom, right, bottom, right, top};
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	GLfloat quadUVs[] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(GL_QUADS, 0, 4);
	
	glDisable(GL_TEXTURE_2D);
}

void eventHandler(bool& done, Entity& ball){
	SDL_Event event;
	//Enable closing of window
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_KEYDOWN){
			//If we press space when the ball is stationary (at the center of the screen)
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && ball.getXspeed() == 0.0f){
				//Move in a random direction
				srand(time(NULL));
				int dice_roll = rand()%6;
				GLfloat directions[] = { 0.0f, 45.0f, 135.0f, 180.0f, 225.0f, 315.0f };
				ball.setSpeed(BALL_SPEED, directions[dice_roll]);
			}
		}
	}
}

void fixPaddle(Entity& paddle){
	
	GLfloat halfHeight = paddle.getHalfHeight();
	//check if the top edge is beyond the top of the screen
	if (paddle.getY() + halfHeight >= 1.0f) { paddle.setY(1.0f - halfHeight); }
	//same for bottom
	if (paddle.getY() - halfHeight <= -1.0f) { paddle.setY(-1.0f + halfHeight); }
}

void resetBall(Entity& ball){
	ball.setPos(0.0f, 0.0f);
	ball.setSpeed(0.0f, 0.0f);
}

void collide(Entity& paddle, Entity& ball, int whichSide, GLfloat top, GLfloat bot, GLfloat mid){
	GLfloat halfWidth = paddle.getHalfWidth();
	GLfloat ballHalfWidth = ball.getHalfWidth();

	bool threshold;
	GLfloat paddleEdge = paddle.getX() + halfWidth*whichSide;
	GLfloat ballEdge = ball.getX() - ballHalfWidth*whichSide;
	if (whichSide == LEFT) { threshold = paddleEdge >= ballEdge; }
	else { threshold = paddleEdge <= ballEdge; }

	if (threshold){
		//Reset condition: you went past the paddle
		GLfloat ballHalfHeight = ball.getHalfHeight();
		GLfloat halfHeight = paddle.getHalfHeight();
		ballEdge = ball.getY() - ballHalfHeight;
		paddleEdge = paddle.getY() + halfHeight;
		if (ballEdge >= paddleEdge){ resetBall(ball); return; }
		ballEdge = ball.getY() + ballHalfHeight;
		paddleEdge = paddle.getY() - halfHeight;
		if (ballEdge <= paddleEdge){ resetBall(ball); return; }

		//Where is the y-val of ball relative to the paddle
		GLfloat ballY = ball.getY();
		GLfloat pY = paddle.getY();
		GLfloat sixth = paddle.getHeight() / 6.0f;

		if (ballY > pY + sixth){ ball.setSpeed(BALL_SPEED, top); }
		else if (ballY < pY - sixth){ ball.setSpeed(BALL_SPEED, bot); }
		else{ ball.setSpeed(BALL_SPEED, mid); }
	}

}

void fixBall(Entity& ball){
	GLfloat ballHalfHeight = ball.getHalfHeight();

	//See if the ball hit bottom of screen
	GLfloat ballEdge = ball.getY() - ballHalfHeight;
	if (ballEdge <= -1.0f) {
		ball.setY(-1.0f + ballHalfHeight);
		ball.setYspeed(ball.getYspeed()*-1);
	}

	//if ball hit top
	ballEdge = ball.getY() + ballHalfHeight;
	if (ballEdge >= 1.0f) {
		ball.setY(1.0f - ballHalfHeight);
		ball.setYspeed(ball.getYspeed()*-1);
	}
}

void update(float& lastTickCount, const Uint8* keys,
	Entity& leftPaddle, Entity& rightPaddle, Entity& ball){
	glClear(GL_COLOR_BUFFER_BIT);

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastTickCount;
	lastTickCount = ticks;

	//Move the ball
	ball.setX(ball.getX() + elapsed * ball.getXspeed());
	ball.setY(ball.getY() + elapsed * ball.getYspeed());
	if (ball.getXspeed()){
		collide(leftPaddle, ball, LEFT, ANG_TR, ANG_BR, ANG_R);
		collide(rightPaddle, ball, RIGHT, ANG_TL, ANG_BL, ANG_L);
	}
	fixBall(ball);

	//Move the left paddle
	keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_W]) { leftPaddle.setY(leftPaddle.getY() + elapsed); }
	if (keys[SDL_SCANCODE_S]) { leftPaddle.setY(leftPaddle.getY() - elapsed); }
	fixPaddle(leftPaddle);

	//Move the right paddle
	if (keys[SDL_SCANCODE_UP]) { rightPaddle.setY(rightPaddle.getY() + elapsed); }
	if (keys[SDL_SCANCODE_DOWN]) { rightPaddle.setY(rightPaddle.getY() - elapsed); }
	fixPaddle(rightPaddle);
}


void render(Entity& instructions, Entity& leftPaddle, Entity& rightPaddle, Entity& ball){
	DrawEntity(instructions);
	DrawEntity(leftPaddle);
	DrawEntity(rightPaddle);
	DrawEntity(ball);
	SDL_GL_SwapWindow(displayWindow);
}

int main(int argc, char *argv[])
{
	/*Housekeeping variables and function calls*/
	setup();
	
	float lastTickCount = 0.0f;
	const Uint8* keys = NULL;

	//Stuff we see
	
	Entity instructions("instructions.png", 0.0f, 0.7f, false);
	Entity leftPaddle("leftPaddle.png", -1.0f, 0.0f, false);
	Entity rightPaddle("rightPaddle.png", 1.0f, 0.0f, false);
	Entity ball("ballGrey.png", 0.0f, 0.0f);

	OutputDebugString("About to enter Main loop");

	//Main loop
	bool done = false;
	while (!done) {
		eventHandler(done, ball);
		
		update(lastTickCount, keys, leftPaddle, rightPaddle, ball);

		render(instructions, leftPaddle, rightPaddle, ball);
	}

	SDL_Quit();
	return 0;
}