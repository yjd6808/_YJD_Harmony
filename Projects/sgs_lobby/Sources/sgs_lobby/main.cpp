
#include "jc/Random.h"

#include "sg/LogSpecifier.h"

#include "sgs/_API/sgapiServerBase.h"
#include "sgs/_Net/NetCore.h"
#include "sgs/_Net/NetGroup_InterServ.h"

#include "sgs_lobby/LobbyCore.h"

USING_NS_JC;
USING_NS_JNET;

int main(int _argc, char** _argv)
{
	new char; // 릭 확인용

	//////////////////////////////////////////////////////////////////////////////////////////
	// 메인 리소스 초기화
	//////////////////////////////////////////////////////////////////////////////////////////
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
	if (g_cNetGroup_InterServ.ConnectCenterServer(999))
	{
		g_cNetCore.ProcessMainLoop();
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
