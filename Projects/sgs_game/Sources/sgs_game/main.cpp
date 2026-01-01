#include "Core.h"
#include "GameCoreHeader.h"

#include <jc/Random.h>

#include <sg/LogSpecifier.h>

USING_NS_JC;
USING_NS_JNET;

int parse_args(int _argc, char* _pArgv[]);

int main(int _argc, char* _pArgv[])
{
    new char;

    //////////////////////////////////////////////////////////////////////////////////////
    // 메인 리소스 초기화
    //////////////////////////////////////////////////////////////////////////////////////
	
	Env::InitArgs(_argc, _pArgv[]);
	int gameServerType = parse_args(_argc, _pArgv);
	const String logSpecifier = StringUtil::Format(LOG_SPECIFIER_GAME, GameServerType::Name[gameServerType]);

	Random::EngineInitialize();
    Winsock::Initialize(2, 2);
    Console::SetSize(800, 400);
    InitializeJCore();
    InitializeNetLogger(logSpecifier.Source());
    InitializeDefaultLogger(logSpecifier.Source());
    InitializeCommonCore();
    InitializeServerCore();
    InitializeServerGameLogo(true, 24);
    InitializeGameCore(static_cast<GameServerType_t>(gameServerType));

    //////////////////////////////////////////////////////////////////////////////////////
    // 메인 루틴
    //////////////////////////////////////////////////////////////////////////////////////

    if (sg::InterServerClientNetGroup && sg::InterServerClientNetGroup->ConnectCenterServer(5))
    {
		sg::NetGroupMgr->ProcessMainUpdate();
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // 메인 리소스 정리
    //////////////////////////////////////////////////////////////////////////////////////

    FinalizeGameCore();
    FinalizeServerCore();
    FinalizeCommonCore();
    FinalizeDefaultLogger();
    FinalizeNetLogger();
    FinalizeJCore();
    Winsock::Finalize();
    return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
int parse_args(int _argc, char* _pArgv[])
{
	if (_argc <= 1)
	{
		// 게임 서버 타입을 인자로 전달해주세요.
		Console::WriteLine("required GameServerType_t(integer) argument");
		::exit(-1);
	}

	int gameServerType = -1;
	if (!StringUtil::TryToNumber<int>(gameServerType, _pArgv[1]))
	{
		// 전달받은 인자를 정수타입으로 변환하는데 실패했습니다. (%s)
		Console::WriteLine("not a integer string. (%s)", _pArgv[1]);
		::exit(-2);
	}

	if (gameServerType < GameServerType::Begin || gameServerType > GameServerType::End)
	{
		// 전달받은 인자가 올바른 게임 서버 타입이 아닙니다. (%d)
		Console::WriteLine("invalid GameServerType. (%d)", gameServerType);
		::exit(-3);
	}

    return gameServerType;
}
