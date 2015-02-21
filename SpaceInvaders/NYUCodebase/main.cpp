#include "GameClass.h"

int main(int argc, char *argv[])
{
	//WindowClass w;
	GameClass g;

	OutputDebugString("About to enter Main loop");

	//Main loop
	//while (w.run()){}
	while (g.run()){}

	return 0;
}