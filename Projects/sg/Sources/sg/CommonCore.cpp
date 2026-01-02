/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:35:17 AM
 * =====================
 *
 */

#include "Core.h"
#include "CommonCore.h"

#include <sg/Cmd.h>
#include <sg/LogSpecifier.h>
#include <jc/Threading/ThreadPool.h>
#include <jc/Threading/Scheduler.h>

#include <sg/CLIThread.h>
#include <sg/CLIListener.h>

#include <sg/_Struct/SteinsGate_Char.h>
#include <sg/_Struct/SteinsGate_Server.h>

#include <sg/_Util/DescLoaderMgr.h>
#include <sg/Config.h>

#include <sg/_Core/VirtualMachine.h>

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
CharCommonInfo* CharCommon;
::ServerProcessInfoPackage* ServerProcessInfoPackage;
jnet::CommandNameDictionary CommandNameDictionary;
::DescLoaderMgr* DescLoaderMgr;
NS_SG_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeCommonCore()
{
	DefaultParserType = PacketParser::Command;

	// 설정 
	g_cConfigArgs.Load();
	g_cConfigRuntime.Load();

	g_cVM.Init();

	// 공통 커맨드 이름 등록
	// [ AUTHENTICATION ]
	sg::CommandNameDictionary.Add<CAU_Login>();
	sg::CommandNameDictionary.Add<AUC_LoginAck>();

	// [ LOBBY ]
	sg::CommandNameDictionary.Add<CLO_JoinLobby>();
	sg::CommandNameDictionary.Add<LOC_JoinLobbyAck>();

	// [ MESSAGE ]
	sg::CommandNameDictionary.Add<SC_ClientText>();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeCommonCore()
{
	g_cVM.Free();

	g_cConfigRuntime.Free();
	g_cConfigArgs.Free();
}

