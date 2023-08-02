#include "Center.h"
#include "CenterCoreHeader.h"

#include <JCore/Logger/ConsoleLogger.h>
#include <JCore/Random.h>

#include <SteinsGate/Common/LogSpecifier.h>

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
	option.EnableLog[LoggerAbstract::eDebug] = true;
	option.EnableLog[LoggerAbstract::eError] = true;
	option.EnableLog[LoggerAbstract::eWarn] = true;
	option.EnableLog[LoggerAbstract::eInfo] = true;
	return option;
}();

int main() {

	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger(&NetLoggerOption_v, LOG_SPECIFIER_CENTER);
	InitializeDefaultLogger(&LoggerOption_v, LOG_SPECIFIER_CENTER);
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerCenterLogo(true, 14);
	InitializeCenterCore();

	{
		CoreServer_v->Start(CoreServerProcessInfoPackage_v->Center.BindInterServerTcp);
		CoreInputThread_v->SetEventMap({
			CenterInputEvent::PairOf(CenterInputEvent::TerminateProgram),
			CenterInputEvent::PairOf(CenterInputEvent::ShutdownAllServer),
			CenterInputEvent::PairOf(CenterInputEvent::ShutdownAuthServer),
			CenterInputEvent::PairOf(CenterInputEvent::ShutdownLobbyServer),
			CenterInputEvent::PairOf(CenterInputEvent::ShutdownGameServer),
			CenterInputEvent::PairOf(CenterInputEvent::ServerState),
		});

		CoreNetMaster_v->ProcessMainUpdate();
	}

	FinalizeCenterCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}