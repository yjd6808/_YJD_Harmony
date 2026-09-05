#include "Core.h"

extern void Test_CoStack();
extern void Test_CoBasic();
extern void Test_Co07_ErrorVeh();
extern void Test_Co05_ResumeSafety();
extern void Test_Co06_CustomTier();
extern void Test_Co01_Exception();
extern void Test_Co02_Teb();
extern void Test_Co03_StackLimit();
extern void Test_Co08_Mxcsr();

//////////////////////////////////////////////////////////////////////////////////////////
int call_06_Coroutine(int _argc, char** _argv)
{
	// Test_CoStack();
	Test_CoBasic();
	Test_Co07_ErrorVeh();
	Test_Co05_ResumeSafety();
	Test_Co06_CustomTier();
	Test_Co01_Exception();
	Test_Co02_Teb();
	Test_Co03_StackLimit();
	Test_Co08_Mxcsr();

	return 0;
}
