/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Core.h"
#include "LobbyCore.h"
#include "LobbyCoreHeader.h"

#include <sg/_Util/DescLoaderMgr.h>
#include <sg/_Util/_DescMgr/DescMgr_Server.h>
#include <sg/_Util/_DescMgr/DescMgr_Database.h>

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
::MysqlDatabase*    GameDB;
::LobbyNetMaster*   NetGroupMgr;
::LobbyNetGroup*    NetGroup;
::LobbyServer*      Server;
::LobbyContents     Contents;
NS_SG_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeLobbyCore()
{
	g_cDescMgr.AddLoader(dbg_new ServerInfoLoader());
	g_cDescMgr.AddLoader(dbg_new DatabaseInfoLoader());
	g_cDescMgr.LoadAll();

	sg::ServerProcessInfoPackage = g_cDescMgr.GetServerProcessInfoPackage();
	sg::ServerProcessInfo = &sg::ServerProcessInfoPackage->lobby_;
	sg::GameDB = dbg_new MysqlDatabase(g_cDescMgr.GetDatabaseInfo(DatabaseType::Game));
	sg::GameDB->Initialize(ServerProcessType::Lobby);
	sg::NetGroupMgr = LobbyNetMaster::Get();
	sg::NetGroupMgr->Initialize();
	sg::NetGroup = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::MainId).Get<LobbyNetGroup*>();
	sg::Server = sg::NetGroup->GetLobbyTcp();

	// COMMON INJECTION
	sg::CommonNetGroupMgr = sg::NetGroupMgr;
	sg::CommonNetGroup = sg::NetGroup;
	sg::CommonServer = sg::Server;
	sg::CommonContents = &sg::Contents;
	sg::InterServerClientNetGroup = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	sg::InterServerClientTcp = sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientTcp() : nullptr;
	sg::InterServerClientUdp = sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientUdp() : nullptr;
	sg::ServerProcessInfo = &sg::ServerProcessInfoPackage->lobby_;
	sg::TimeManager = TimeManager::Get();

	sg::Contents.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeLobbyCore()
{
	sg::NetGroupMgr->Finalize();
	sg::Contents.Finalize();

	JC_DELETE_SAFE(sg::GameDB);
	JC_DELETE_SINGLETON_SAFE(sg::TimeManager);
	JC_DELETE_SINGLETON_SAFE(sg::NetGroupMgr);
	g_cDescMgr.Free();
}
