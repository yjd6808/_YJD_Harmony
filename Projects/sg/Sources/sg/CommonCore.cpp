/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:35:17 AM
 * =====================
 *
 */

#include "CommonCore.h"

#include "sg/Cmd.h"
#include "sg/LogSpecifier.h"
#include "jc/Threading/ThreadPool.h"
#include "jc/Threading/Scheduler.h"

#include "sg/Core/CLIThread.h"
#include "sg/Core/CLIListener.h"

#include "sg/Struct/SteinsGate_Char.h"
#include "sg/Struct/SteinsGate_Server.h"

#include "sg/Util/DescLoaderMgr.h"
#include "sg/Core/AppConfig.h"

#include "sg/Core/VirtualMachine.h"

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
CharCommonInfo* CharCommon;
jnet::CommandNameDictionary CommandNameDictionary;
NS_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeCommonCore()
{
	g_cRandom.Initialize();

	// 설정
	g_cAppConfig.ReadEnvArgs();
	g_cAppConfig.LoadConfFile();

	_LogDebug_("[DBG] Before g_cVM.Init");
	g_cVM.Init();
	_LogDebug_("[DBG] After g_cVM.Init");

	// 공통 커맨드 이름 등록
	// [ AUTHENTICATION ]
	sg::CommandNameDictionary.Add<CAU_Login>();
	_LogDebug_("[DBG] After Add CAU_Login");
	sg::CommandNameDictionary.Add<AUC_LoginAck>();
	_LogDebug_("[DBG] After Add AUC_LoginAck");

	// [ LOBBY ]
	sg::CommandNameDictionary.Add<CLO_JoinLobby>();
	_LogDebug_("[DBG] After Add CLO_JoinLobby");
	sg::CommandNameDictionary.Add<LOC_JoinLobbyAck>();
	_LogDebug_("[DBG] After Add LOC_JoinLobbyAck");

	// [ MESSAGE ]
	sg::CommandNameDictionary.Add<SC_ClientText>();
	_LogDebug_("[DBG] After Add SC_ClientText");
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeCommonCore()
{
	g_cVM.Free();

	g_cAppConfig.SaveConfiFile();
	g_cAppConfig.Free();
}

