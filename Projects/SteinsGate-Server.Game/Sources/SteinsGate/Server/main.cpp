#include "Game.h"
#include "GameCoreHeader.h"

#include <JCore/Random.h>

#include <SteinsGate/Common/LogSpecifier.h>

USING_NS_JC;
USING_NS_JNET;

int parse_args(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	new char;

	// ======================================================
	// 메인 리소스 초기화
	// ======================================================

	int eGameServerType = parse_args(argc, argv);
	const String szLogSpecifier = StringUtil::Format(LOG_SPECIFIER_GAME, GameServerType::Name[eGameServerType]);
	Random::EngineInitialize();
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeNetLogger(szLogSpecifier.Source());
	InitializeDefaultLogger(szLogSpecifier.Source());
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerGameLogo(true, 24);
	InitializeGameCore((GameServerType_t)eGameServerType);

	// ======================================================
	// 메인 루틴
	// ======================================================

	if (Core::InterServerClientNetGroup && Core::InterServerClientNetGroup->ConnectCenterServer(5)) {
		Core::NetMaster->ProcessMainUpdate();
	}

	// ======================================================
	// 메인 리소스 정리
	// ======================================================

	FinalizeGameCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	Winsock::Finalize();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}


int parse_args(int argc, char* argv[]) {
	if (argc == 0) {
		Console::WriteLine("게임 서버 타입을 인자로 전달해주세요");
		::exit(-1);
	}

	int eGameServerType = -1;
	if (!StringUtil::TryToNumber<int>(eGameServerType, argv[1])) {
		Console::WriteLine("전달받은 인자를 정수타입으로 변환하는데 실패했습니다. (%s)", argv[1]);
		::exit(-2);
	}

	if (eGameServerType < GameServerType::Begin || eGameServerType > GameServerType::End) {
		Console::WriteLine("전달받은 인자가 올바른 게임 서버 타입이 아닙니다. (%d)", eGameServerType);
		::exit(-3);
	}

	return eGameServerType;
}