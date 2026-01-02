/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "GameCore.h"
#include "GameCoreHeader.h"

#include <sgs_game/CLIListener.h>

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
::DataManager*				DataManager;
::MysqlDatabase*			GameDB;
::GameNetMaster*			NetGroupMgr;
::GameNetGroup*				NetGroup;
::LogicServer*				Server;
::GameServerType_t			GameServerType;
::GameServerProcessInfo*	GameServerProcessInfo;
::RuntimeConfig*			RuntimeConfig;
NS_SG_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeGameCore(GameServerType_t _gameServerType)
{
	sg::GameServerType			= _gameServerType;
	sg::ServerProcessInfoPackage	= g_cDescMgr.GetServerProcessInfoPackage(1);					// 공통 라이브러리 주입
	sg::GameServerProcessInfo		= sg::ServerProcessInfoPackage->GetGameServerProcessInfo(_gameServerType);
	sg::ServerProcessInfo			= sg::GameServerProcessInfo;		// 공통 라이브러리 주입
	sg::GameDB					= dbg_new MysqlDatabase(g_cDescMgr.GetDatabaseInfo(DatabaseType::Game));
	sg::GameDB->Initialize(ServerProcessType::Game);
	sg::NetGroupMgr					= GameNetMaster::Get();
	sg::NetGroupMgr->SetProcessInfo(sg::ServerProcessInfo);
	sg::NetGroupMgr->Initialize();
	sg::NetGroup					= sg::NetGroupMgr->GetNetGroup(Const::NetGroup::MainId).Get<GameNetGroup*>();
	sg::InterServerClientNetGroup = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	sg::Server					= sg::NetGroup->GetLogicTcp();
	sg::RuntimeConfig				= RuntimeConfig::Get();
	sg::RuntimeConfig->Load();

	// BASE INJECTION
	sg::CharCommon					= nullptr;											// 사용안함

	// COMMON INJECTION
	sg::CommonNetGroupMgr				= sg::NetGroupMgr;
	sg::CommonNetGroup				= sg::NetGroup;
	sg::CommonServer					= sg::Server;
	sg::CommonRuntimeConfig			= sg::RuntimeConfig;
	sg::CommonContents				= &sg::Contents;
	sg::InterServerClientNetGroup		= sg::NetGroupMgr->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	sg::InterServerClientTcp			= sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientTcp() : nullptr;
	sg::InterServerClientUdp			= sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientUdp() : nullptr;
	sg::ServerProcessInfo				= sg::GameServerProcessInfo;						// 위에서 주입됨
	sg::TimeManager					= TimeManager::Get();

	sg::Contents.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeGameCore()
{
	JC_DELETE_SAFE(sg::GameDB);
	JC_DELETE_SINGLETON_SAFE(sg::NetGroupMgr);
	JC_DELETE_SINGLETON_SAFE(sg::TimeManager);
	JC_DELETE_SINGLETON_SAFE(sg::RuntimeConfig);
}
