#include "jc/Env.h"
#include "jc/Container/PropertyStatics.h"

#include <windows.h>

int main(int _argc, char** _argv)
{
	::SetCurrentDirectory(Env::CurrentDirectory().Source());
	::testing::InitGoogleTest(&_argc, _argv);
	InitializeJCore(_argc, _argv);
	const int ret = RUN_ALL_TESTS();
	PropertyStatics::Finalize();
	FinalizeJCore();
	return ret;
}
