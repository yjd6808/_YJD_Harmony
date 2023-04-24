#include "Center.h"
#include "CenterCoreHeader.h"

#include <JCore/Logger/ConsoleLogger.h>

USING_NS_JC;
USING_NS_JNET;

ConsoleLoggerOption LoggerOption_v = [] {
	ConsoleLoggerOption option;
	option.EnableLog[LoggerAbstract::eDebug] = true;
	option.EnableLog[LoggerAbstract::eError] = true;
	option.EnableLog[LoggerAbstract::eWarn] = true;
	option.EnableLog[LoggerAbstract::eInfo] = true;
	return option;
}();

ConsoleLoggerOption NetLoggerOption_v = [] {
	ConsoleLoggerOption option;
	option.EnableLog[LoggerAbstract::eDebug] = false;
	option.EnableLog[LoggerAbstract::eError] = true;
	option.EnableLog[LoggerAbstract::eWarn] = true;
	option.EnableLog[LoggerAbstract::eInfo] = true;
	return option;
}();

int main() {

	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger(&NetLoggerOption_v);
	InitializeDefaultLogger(&LoggerOption_v);
	InitializeServerCore();
	InitializeServerCenterLogo(true, 14);
	InitializeCenterCore();

	{
		CoreServer_v->Start(CoreServerProcessInfo_v->Center.BindCenterTcp);
		CoreInputThread_v->SetEventMap({
			CenterInputEvent::PairOf(CenterInputEvent::TerminateProgram),
			CenterInputEvent::PairOf(CenterInputEvent::ShutdownAllServer),
			CenterInputEvent::PairOf(CenterInputEvent::ShutdownAuthServer),
			CenterInputEvent::PairOf(CenterInputEvent::ShutdownLobbyServer),
			CenterInputEvent::PairOf(CenterInputEvent::ShutdownGameServer),
			CenterInputEvent::PairOf(CenterInputEvent::ServerState),
		});

		CoreNetMaster_v->ProcessMainLoop();
	}

	FinalizeCenterCore();
	FinalizeServerCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}