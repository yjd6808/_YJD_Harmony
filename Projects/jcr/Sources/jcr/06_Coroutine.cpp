#include "Core.h"
#include "06_Coroutine_Menu.h"

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
	Test_Co04_Emergency();
	Test_Co15_VehDefense();
	Test_Co14_Api();

	return 0;
}
