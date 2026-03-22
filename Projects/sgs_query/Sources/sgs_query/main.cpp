#include "sg/LogSpecifier.h"

#include "sgs/_API/sgapiServerBase.h"
#include "sgs/_Net/NetCore.h"
#include "sgs/_Net/NetGroup_InterServ.h"

#include "sgs_query/QueryCore.h"

USING_NS_JC;
USING_NS_JNET;

int main(int _argc, char** _argv)
{
	new char;

	//////////////////////////////////////////////////////////////////////////////////////
	// 메인 리소스 초기화
	//////////////////////////////////////////////////////////////////////////////////////

	Winsock::Initialize(2, 2);
	Console::SetSize(800, 400);
	InitializeJCore(_argc, _argv);
	InitializeNetLogger(LOG_SPECIFIER_QUERY);
	InitializeDefaultLogger(LOG_SPECIFIER_QUERY);
	sgapiBase::Init(dbg_new sgapiServerBase);
	InitializeCommonCore();
	InitializeServerCore();
	InitializeServerQueryLogo(true, 24);
	InitializeQueryCore();

	//////////////////////////////////////////////////////////////////////////////////////
	// 메인 루틴
	//////////////////////////////////////////////////////////////////////////////////////

	if (g_cNetGroup_InterServ.ConnectCenterServer(999))
	{
		g_cNetCore.ProcessMainLoop();
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// 메인 리소스 정리
	//////////////////////////////////////////////////////////////////////////////////////

	FinalizeQueryCore();
	FinalizeServerCore();
	FinalizeCommonCore();
	FinalizeDefaultLogger();
	FinalizeNetLogger();
	FinalizeJCore();
	Winsock::Finalize();
	sgapiBase::Free();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X);
}
