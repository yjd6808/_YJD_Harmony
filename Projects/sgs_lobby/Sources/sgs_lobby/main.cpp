#include "Core.h"
#include "LobbyCoreHeader.h"

#include <jc/Random.h>

#include <sgs_lobby/LobbyContents.h>
#include <sg/LogSpecifier.h>

#include <sgs/_API/sgapiServerBase.h>

USING_NS_JC;
USING_NS_JNET;

int main(int _argc, char** _argv)
{
	new char; // 릭 확인용

	//////////////////////////////////////////////////////////////////////////////////////////
	// 메인 리소스 초기화
	//////////////////////////////////////////////////////////////////////////////////////////
	Random::EngineInitialize();
	sgapiBase::Init(dbg_new sgapiServerBase);
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeJCore(_argc, _argv);
	InitializeNetLogger(LOG_SPECIFIER_LOBBY);
	InitializeDefaultLogger(LOG_SPECIFIER_LOBBY);
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerLobbyLogo(true, 24);
	InitializeLobbyCore();

	//////////////////////////////////////////////////////////////////////////////////////////
	// 메인 루틴
	//////////////////////////////////////////////////////////////////////////////////////////
	if (sg::NetGroup_InterServ && sg::NetGroup_InterServ->ConnectCenterServer(5))
	{
		sg::NetGroupMgr->ProcessMainUpdate();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// 메인 리소스 정리
	//////////////////////////////////////////////////////////////////////////////////////////
	FinalizeLobbyCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	FinalizeJCore();
	Winsock::Finalize();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
