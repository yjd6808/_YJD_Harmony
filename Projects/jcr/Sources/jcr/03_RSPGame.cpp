#include "Core.h"

extern "C"
{
	void RSPGame_main();
}

//////////////////////////////////////////////////////////////////////////////////////////
int call_03_RSPGame(int _argc, char** _argv)
{
	RSPGame_main();
	return 0;
}
