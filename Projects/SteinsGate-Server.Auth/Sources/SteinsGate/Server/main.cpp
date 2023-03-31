#include "Auth.h"
#include "AuthCoreHeader.h"

USING_NS_JC;
USING_NS_JNET;




int main() {
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeServerAuthLogo(true, 24);
	InitializeDefaultLogger();
	InitializeAuthCore();

	{
		CoreNetMaster_v->MainLoop();
	}
	
	FinalizeAuthCore();
	FinalizeDefaultLogger();
	Winsock::Finalize();
	return 0;
}