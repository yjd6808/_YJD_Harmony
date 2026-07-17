#include "Core.h"

extern void Test_CoStack();
extern void Test_CoBasic();

//////////////////////////////////////////////////////////////////////////////////////////
int call_06_Coroutine(int _argc, char** _argv)
{
	// Test_CoStack();
	Test_CoBasic();

	return 0;
}
