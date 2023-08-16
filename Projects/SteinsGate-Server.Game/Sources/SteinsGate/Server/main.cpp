#include "Game.h"
#include "GameCoreHeader.h"

#include <JCore/Logger/ConsoleLogger.h>
#include <JCore/Random.h>

#include <SteinsGate/Server/GameContents.h>
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

int main(int argc, char* argv[]) {
	new char;
	if (argc == 0) {
		Console::WriteLine("게임 서버 타입을 인자로 전달해주세요");
		return -1;
	}

	int eGameServerType = -1;
	if (!StringUtil::TryToNumber<int>(eGameServerType, argv[1])) {
		Console::WriteLine("전달받은 인자를 정수타입으로 변환하는데 실패했습니다. (%s)", argv[1]);
		return -2;
	}

	if (eGameServerType < GameServerType::Begin || eGameServerType > GameServerType::End) {
		Console::WriteLine("전달받은 인자가 올바른 게임 서버 타입이 아닙니다. (%d)", eGameServerType);
		return -3;
	}

	const String szLogSpecifier = StringUtil::Format(LOG_SPECIFIER_GAME, GameServerType::Name[eGameServerType]);
	Random::EngineInitialize();
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger(&NetLoggerOption_v, szLogSpecifier.Source());
	InitializeDefaultLogger(&LoggerOption_v, szLogSpecifier.Source());
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerGameLogo(true, 24);

	if (!InitializeGameCore((GameServerType_t)eGameServerType)) {
		Console::WriteLine("활성화되지 않은 게임서버타입입니다. (%s)", GameServerType::Name[eGameServerType]);
		return -4;
	}
	InitializeGameContents();

	{
		if (CoreInterServerClientNetGroup_v->ConnectCenterServer(5)) {
			CoreNetMaster_v->ProcessMainUpdate();
		}
	}

	FinalizeGameContents();
	FinalizeGameCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}
