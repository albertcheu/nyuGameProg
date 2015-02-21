#include "GameClass.h"

int main(int argc, char *argv[])
{
	GameClass gc;

	OutputDebugString("About to enter Main loop");

	//Main loop
	while (gc.run()){}

	return 0;
}