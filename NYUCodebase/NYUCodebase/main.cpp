#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

SDL_Window* displayWindow;

void housekeeping(){
	//Boilerplate
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//Establish how big our screen is
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	//This is a 2D game, so it ought to be orthographic
	//Adjust the scale (ought to fit the screen!)
	glOrtho(-1.33, 1.33, -1, 1, -1, 1);
	
	//Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void gradientCode(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.5f, -0.5f, 0);

	//What are we going to draw?
	GLfloat tri[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0, 0.5f };

	//What color(s) is it going to be?
	GLfloat triColors[] = {
		1.0f, 1.0f, 0.0f,//yellow
		1.0f, 0.6f, 0.0f,//orange
		1.0f, 0.0f, 0.0f//dark red
	};

	//Actually draw our vertices
	glVertexPointer(2, GL_FLOAT, 0, tri);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	//And then their colors
	glColorPointer(3, GL_FLOAT, 0, triColors);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_COLOR, 0, 3);
	
}

typedef struct{
	GLuint id;
	GLint width;
	GLint height;
} TextureData;

TextureData LoadTexture(const char *image_path, GLint internalFormat, GLenum format) {
	SDL_Surface *surface = IMG_Load(image_path);
	if (!surface) {	OutputDebugString("Error loading image"); }

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

TextureData LoadTextureRGB(const char* image_path) { return LoadTexture(image_path, GL_RGB, GL_RGB); }
TextureData LoadTextureRGBA(const char* image_path) { return LoadTexture(image_path, GL_RGBA, GL_RGBA); }

void DrawSprite(GLint texture, float x, float y, float angle, GLfloat width, GLfloat height) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(angle, 0.0, 0.0, 1.0);

	GLfloat left, right, top, bottom;
	left = width * -0.5f;
	right = width * 0.5f;
	bottom= height* -0.5f;
	top = height * 0.5f;

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

int main(int argc, char *argv[])
{
	/*Housekeeping variables and function calls*/
	housekeeping();
	bool done = false;
	SDL_Event event;
	float lastTickCount = 0.0f;

	TextureData ship = LoadTextureRGBA("player.png");
	GLfloat shipAngle = 0.0f;
	TextureData ball = LoadTextureRGBA("ballBlue.png");
	TextureData box = LoadTextureRGBA("box.png");

	//Main loop
	while (!done) {
		//Enable closing of window
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		
		glClear(GL_COLOR_BUFFER_BIT);

		float ticks = (float)SDL_GetTicks() / 100.0f;
		float elapsed = ticks - lastTickCount;
		lastTickCount = ticks;

		shipAngle += elapsed;
		shipAngle = (shipAngle > 360.0f) ? shipAngle-360.0f: shipAngle;

		gradientCode();
		
		DrawSprite(ship.id, 0.4f, 0.4f, shipAngle, ship.width/100.0f,ship.height/100.0f);
		DrawSprite(ball.id, -0.5f, -0.5f, 0.0, ball.width / 100.0f, ball.height / 100.0f);
		DrawSprite(box.id, -0.5f, 0.5f, 0.0, box.width / 100.0f, box.height / 100.0f);

		//Render the window
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}