#include "Core.h"
#include "AuthCoreHeader.h"

#include <jc/Random.h>

#include <sgs_auth/AuthContents.h>
#include <sg/LogSpecifier.h>

USING_NS_JC;
USING_NS_JNET;

int main()
{
	new char;

	//////////////////////////////////////////////////////////////////////////////////////
	// 메인 리소스 초기화
	//////////////////////////////////////////////////////////////////////////////////////

	Random::EngineInitialize();
	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeJCore();
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerAuthLogo(true, 24);
	InitializeAuthCore();

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

	FinalizeAuthCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	FinalizeJCore();
	Winsock::Finalize();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X);
}