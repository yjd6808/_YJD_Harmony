#include "Auth.h"
#include "AuthCoreHeader.h"

#include <JCore/Logger/ConsoleLogger.h>
#include <JCore/Random.h>
#include <SteinsGate/Server/AuthContents.h>

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
	Random::EngineInitialize();
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger(&NetLoggerOption_v);
	InitializeDefaultLogger(&LoggerOption_v);
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerAuthLogo(true, 24);
	InitializeAuthCore();
	InitializeAuthContents();

	{
		CoreInputThread_v->SetEventMap({
			AuthInputEvent::PairOf(AuthInputEvent::TerminateProgram)
		});

		if (CoreNetGroup_v->ConnectCenterServer(5)) {
			CoreNetMaster_v->ProcessMainLoop();
		}
	}

	FinalizeAuthContents();
	FinalizeAuthCore();
	FinalizeServerCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}
