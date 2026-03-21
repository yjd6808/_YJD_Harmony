#include "jnetr_common/PCH/Core.h"
#include "jnetr_common/ServerNetGroupMgr.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
int main(int _argc, char** _argv)
{
	Winsock::Initialize(2, 2);
	InitializeJCore(_argc, _argv);
	InitializeNetLogger();
	{
		ServerNetGroupMgr mgr;
		mgr.Initialize();

		while (getchar() != 'c')
		{
		}

	}
	FinalizeNetLogger();
	FinalizeJCore();
	Winsock::Finalize();
	return 0;
}
