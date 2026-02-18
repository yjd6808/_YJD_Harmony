/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Core.h"
#include "CenterCore.h"
#include "CenterCoreHeader.h"

#include <sg/_Util/DescLoaderMgr.h>
#include <sg/_Util/_DescMgr/DescMgr_ServerInfo.h>
#include <sg/_Util/_DescMgr/DescMgr_Database.h>

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
::DataManager*		DataManager;
::MysqlDatabase*	GameDB;
::CenterNetMaster*	NetGroupMgr;
::CenterNetGroup*	NetGroup;
::CenterServer*		Server;
::CenterContents	Contents;
NS_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeCenterCore() 
{
	g_cDescMgr.AddLoader(dbg_new ServerInfoLoader());
	g_cDescMgr.AddLoader(dbg_new DatabaseInfoLoader());
	g_cDescMgr.LoadAll();

	sg::ServerProcessInfoPackage = g_cDescMgr.GetServerProcessInfoPackage();
	sg::ServerProcessInfo = &sg::ServerProcessInfoPackage->center_;
	sg::GameDB = dbg_new MysqlDatabase(g_cDescMgr.GetDatabaseInfo(DatabaseType::Game));
	sg::GameDB->Initialize(ServerProcessType::Center);
	sg::NetGroupMgr = CenterNetMaster::Get();
	sg::NetGroupMgr->Initialize();
	sg::NetGroup = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::MainId).Get<CenterNetGroup*>();
	sg::Server = sg::NetGroup->GetCenterTcp();
	
	sg::CommonNetGroupMgr = sg::NetGroupMgr;
	sg::CommonNetGroup = sg::NetGroup;
	sg::CommonServer = sg::Server;
	sg::CommonContents = &sg::Contents;
	sg::NetGroup_InterServ = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::InterServerId).Get<NetGroup_InterServ*>();
	sg::InterServerClientTcp = sg::NetGroup_InterServ ? sg::NetGroup_InterServ->GetInterServerClientTcp() : nullptr;
	sg::InterServerClientUdp = sg::NetGroup_InterServ ? sg::NetGroup_InterServ->GetInterServerClientUdp() : nullptr;
	sg::TimeManager = TimeManager::Get();

	sg::Contents.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeCenterCore() 
{
	sg::NetGroupMgr->Finalize();
	sg::Contents.Finalize();

	JC_DELETE_SAFE(sg::GameDB);
	JC_DELETE_SINGLETON_SAFE(sg::TimeManager);
	JC_DELETE_SINGLETON_SAFE(sg::NetGroupMgr);
	g_cDescMgr.Free();
}