/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "LobbyCore.h"
#include "LobbyCoreHeader.h"

#include <sgs_lobby/CLIListener.h>

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
::MysqlDatabase*    GameDB;
::LobbyNetMaster*   NetGroupMgr;
::LobbyNetGroup*    NetGroup;
::LobbyServer*      Server;
::RuntimeConfig*    RuntimeConfig;
::LobbyContents     Contents;
NS_SG_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeLobbyCore()
{
	sg::DataManager              = DataManager::Get();
	sg::ServerProcessInfoPackage = sg::DataManager->GetServerProcessInfoPackage(1);               // 공통 라이브러리 주입
	sg::ServerProcessInfo        = &sg::ServerProcessInfoPackage->lobby_;                           // 공통 라이브러리 주입
	sg::GameDB                   = dbg_new MysqlDatabase(sg::DataManager->GetDatabaseInfo(DatabaseType::Game));
	sg::GameDB->Initialize(ServerProcessType::Lobby);
	sg::NetGroupMgr                = LobbyNetMaster::Get();
	sg::NetGroupMgr->SetProcessInfo(sg::ServerProcessInfo);
	sg::NetGroupMgr->Initialize();
	sg::NetGroup                 = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::MainId).Get<LobbyNetGroup*>();
	sg::Server                   = sg::NetGroup->GetLobbyTcp();
	sg::RuntimeConfig            = RuntimeConfig::Get();
	sg::RuntimeConfig->Load();

	// BASE INJECTION
	if (sg::CLIThread)
		sg::CLIThread->SetListener(dbg_new CLIListener);

	sg::ServerProcessInfoPackage     = sg::DataManager->GetServerProcessInfoPackage(1);       // 위에서 주입됨
	sg::CharCommon                   = nullptr;                                                 // 사용안함
	sg::ThreadPool                   = dbg_new ThreadPool{ 2 };
	sg::Scheduler                    = dbg_new Scheduler{ 2 };
	sg::RuntimeConfig            = sg::RuntimeConfig;

	// COMMON INJECTION
	sg::CommonNetGroupMgr            = sg::NetGroupMgr;
	sg::CommonNetGroup               = sg::NetGroup;
	sg::CommonServer                 = sg::Server;
	sg::CommonRuntimeConfig          = sg::RuntimeConfig;
	sg::CommonContents               = &sg::Contents;
	sg::InterServerClientNetGroup    = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	sg::InterServerClientTcp         = sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientTcp() : nullptr;
	sg::InterServerClientUdp         = sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientUdp() : nullptr;
	sg::ServerProcessInfo            = &sg::ServerProcessInfoPackage->lobby_;                  // 위에서 주입됨
	sg::TimeManager                  = TimeManager::Get();

	sg::Contents.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeLobbyCore()
{
	sg::Contents.Finalize();

	JCORE_DELETE_SAFE(sg::GameDB);
	JCORE_DELETE_SINGLETON_SAFE(sg::TimeManager);
	JCORE_DELETE_SINGLETON_SAFE(sg::RuntimeConfig);
	JCORE_DELETE_SINGLETON_SAFE(sg::NetGroupMgr);
}
