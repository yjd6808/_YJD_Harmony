#include "Game.h"
#include "GameCoreHeader.h"

#include <JCore/Random.h>

#include <SteinsGate/Server/GameContents.h>
#include <SteinsGate/Common/LogSpecifier.h>

USING_NS_JC;
USING_NS_JNET;

int main(int argc, char* argv[]) {
	new char;

	// ======================================================
	// 메인 리소스 초기화
	// ======================================================

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
	InitializeNetLogger(szLogSpecifier.Source());
	InitializeDefaultLogger(szLogSpecifier.Source());
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerGameLogo(true, 24);

	if (!InitializeGameCore((GameServerType_t)eGameServerType)) {
		Console::WriteLine("활성화되지 않은 게임서버타입입니다. (%s)", GameServerType::Name[eGameServerType]);
		return -4;
	}
	InitializeGameContents();

	// ======================================================
	// 메인 루틴
	// ======================================================

	if (CoreInterServerClientNetGroup_v && CoreInterServerClientNetGroup_v->ConnectCenterServer(5)) {
		CoreNetMaster_v->ProcessMainUpdate();
	}

	// ======================================================
	// 메인 리소스 정리
	// ======================================================

	FinalizeGameContents();
	FinalizeGameCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return 0;
}
