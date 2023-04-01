#include "Center.h"
#include "CenterCoreHeader.h"

USING_NS_JC;
USING_NS_JNET;




int main() {
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeServerCenterLogo(true, 14);
	InitializeDefaultLogger();
	InitializeCenterCore();

	{
		CoreNetMaster_v->MainLoop();
	}
	
	FinalizeCenterCore();
	FinalizeDefaultLogger();
	Winsock::Finalize();
	return 0;
}