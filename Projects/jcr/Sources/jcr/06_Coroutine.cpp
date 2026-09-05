#include "Core.h"

extern void Test_CoStack();
extern void Test_CoBasic();
extern void Test_Co07_ErrorVeh();
extern void Test_Co05_ResumeSafety();

//////////////////////////////////////////////////////////////////////////////////////////
int call_06_Coroutine(int _argc, char** _argv)
{
	// Test_CoStack();
	Test_CoBasic();
	Test_Co07_ErrorVeh();
	Test_Co05_ResumeSafety();

	return 0;
}
