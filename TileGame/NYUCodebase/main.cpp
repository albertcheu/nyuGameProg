#include "GameClass.h"

int main(int argc, char *argv[])
{
	GameClass g;
	
	//Main loop
	while (true){
		if (!g.run()) { break; }
	}

	return 0;
}