/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCore.h"
#include "AuthCoreHeader.h"

#include <sg/_Util/DescLoaderMgr.h>
#include <sg/_Util/_DescMgr/DescMgr_Server.h>
#include <sg/_Util/_DescMgr/DescMgr_Database.h>

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
::MysqlDatabase*    GameDB;
::AuthNetMaster*    NetGroupMgr;
::AuthNetGroup*     NetGroup;
::AuthServer*       Server;
::AuthContents      Contents;
NS_SG_END

////////////////////////////////////////////////////////////////////////////////////////
void InitializeAuthCore()
{
	g_cDescMgr.AddLoader(dbg_new ServerInfoLoader());
	g_cDescMgr.AddLoader(dbg_new DatabaseInfoLoader());
	g_cDescMgr.LoadAll();

	sg::ServerProcessInfoPackage        = g_cDescMgr.GetServerProcessInfoPackage();
	sg::ServerProcessInfo               = &sg::ServerProcessInfoPackage->auth_;
	sg::GameDB                          = dbg_new MysqlDatabase(g_cDescMgr.GetDatabaseInfo(DatabaseType::Game));
	sg::GameDB->Initialize(ServerProcessType::Auth);
	sg::NetGroupMgr                       = AuthNetMaster::Get();
	sg::NetGroupMgr->Initialize();
	sg::NetGroup                        = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::MainId).Get<AuthNetGroup*>();
	sg::Server	                        = sg::NetGroup->GetAuthTcp();

	// COMMON INJECTION
	sg::CommonNetGroupMgr               = sg::NetGroupMgr;
	sg::CommonNetGroup                  = sg::NetGroup;
	sg::CommonServer                    = sg::Server;
	sg::CommonContents                  = &sg::Contents;
	sg::InterServerClientNetGroup       = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	sg::InterServerClientTcp            = sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientTcp() : nullptr;
	sg::InterServerClientUdp            = sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientUdp() : nullptr;
	sg::ServerProcessInfo               = &sg::ServerProcessInfoPackage->auth_;
	sg::TimeManager                     = TimeManager::Get();

	sg::Contents.Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////
void FinalizeAuthCore()
{
	sg::NetGroupMgr->Finalize();
	sg::Contents.Finalize();

	JC_DELETE_SAFE(sg::GameDB);
	JC_DELETE_SINGLETON_SAFE(sg::TimeManager);
	JC_DELETE_SINGLETON_SAFE(sg::NetGroupMgr);
	g_cDescMgr.Free();
}
