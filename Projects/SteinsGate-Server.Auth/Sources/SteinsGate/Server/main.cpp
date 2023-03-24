#include "Auth.h"
#include "AuthCoreHeader.h"

USING_NS_JC;
USING_NS_JNET;

int main() {
	Winsock::Initialize(2, 2);
	Console::SetSize(600, 400);
	InitializeDefaultLogger();
	InitializeServerAuthLogo();
	InitializeAuthCore();

	{
		_LogPlain_(Logo_v);
		CoreNetMaster_v->MainLoop();
	}
	
	FinalizeAuthCore();
	FinalizeDefaultLogger();
	Winsock::Finalize();
	return 0;
}