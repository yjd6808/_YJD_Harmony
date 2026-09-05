#include "Core.h"

extern void Test_CoStack();
extern void Test_CoBasic();
extern void Test_Co07_ErrorVeh();

//////////////////////////////////////////////////////////////////////////////////////////
int call_06_Coroutine(int _argc, char** _argv)
{
	// Test_CoStack();
	Test_CoBasic();
	Test_Co07_ErrorVeh();

	return 0;
}
