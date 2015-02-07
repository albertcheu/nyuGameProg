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
	//Adjust the scale (ought to fit the screen!)
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1, 1, -1, 1);
	//Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void gradientSquareCode(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//What are we going to draw?
	GLfloat square[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
	//What color(s) is it going to be?
	GLfloat squareColors[] = {
		1.0f, 1.0f, 0.0f,//yellow
		1.0f, 0.55f, 0.0f,//orange
		0.8f, 0.0f, 0.0f,//dark red
		0.0f, 0.7f, 1.0f//sky blue
	};

	//Actually draw our vertices
	glVertexPointer(2, GL_FLOAT, 0, square);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);

	//And then their colors
	glColorPointer(3, GL_FLOAT, 0, squareColors);
	glEnableClientState(GL_COLOR_ARRAY);
	glDrawArrays(GL_COLOR, 0, 4);

}

GLuint LoadTexture(const char *image_path, GLint internalFormat, GLenum format) {
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
	
	SDL_FreeSurface(surface);
	
	return textureID;
}

GLuint LoadTextureRGB(const char* image_path) { return LoadTexture(image_path, GL_RGB, GL_RGB); }
GLuint LoadTextureRGBA(const char* image_path) { return LoadTexture(image_path, GL_RGBA, GL_RGBA); }

void DrawSprite(GLint texture, float x, float y, float angle) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(angle, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
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

	GLuint ship = LoadTextureRGBA("player.png");
	GLfloat shipAngle = 0.0f;
	GLuint ball = LoadTextureRGBA("ballBlue.png");
	GLuint box = LoadTextureRGBA("box.png");

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
		
		DrawSprite(ship, 0.4f, 0.4f, shipAngle);
		DrawSprite(ball, -0.5f, -0.5f, 0.0);
		DrawSprite(box, -0.5f, 0.5f, 0.0);

		//gradientSquareCode();

		//Render the window
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}