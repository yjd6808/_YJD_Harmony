#include "Auth.h"
#include "AuthCoreHeader.h"

USING_NS_JC;
USING_NS_JNET;

int main() {

	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger();
	InitializeServerCore();
	InitializeServerAuthLogo(true, 24);
	InitializeDefaultLogger();
	InitializeAuthCore();

	{
		NetLogger_v->SetEnableLog(LoggerAbstract::eDebug, true);
		NetLogger_v->SetEnableLog(LoggerAbstract::eError, true);
		NetLogger_v->SetEnableLog(LoggerAbstract::eWarn, true);
		NetLogger_v->SetEnableLog(LoggerAbstract::eInfo, true);

		CoreInputThread_v->SetEventMap({
			AuthInputEvent::PairOf(AuthInputEvent::TerminateProgram)
		});

		if (CoreNetGroup_v->ConnectCenterServer(3)) {
			CoreNetMaster_v->MainLoop();
		}
	}
	
	FinalizeAuthCore();
	FinalizeDefaultLogger();
	FinalizeServerCore();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}