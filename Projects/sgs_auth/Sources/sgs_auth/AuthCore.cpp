/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Core.h"
#include "AuthCore.h"
#include "AuthCoreHeader.h"

#include <jdb/MySQL/MysqlDatabase.h>

#include <sg/_Util/DescLoaderMgr.h>
#include <sg/_Util/_DescMgr/DescMgr_ServerInfo.h>
#include <sg/_Util/_DescMgr/DescMgr_Database.h>

USING_NS_JC;
USING_NS_JNET;
USING_NS_JDB;

NS_SG_BEGIN
::MysqlDatabase*    GameDB;
::AuthNetMaster*    NetGroupMgr;
::AuthNetGroup*     NetGroup;
::AuthServer*       Server;
::AuthContents      Contents;
NS_END

////////////////////////////////////////////////////////////////////////////////////////
void InitializeAuthCore()
{
	DatabaseInfo* pDbInfo = g_cDescMgr.GetDatabaseInfo(DatabaseType::Game);

	g_cNetGroup_InterServ.Initialize();

	sg::ServerProcessInfoPackage        = g_cDescMgr.GetServerProcessInfoPackage();
	sg::ServerProcessInfo               = &sg::ServerProcessInfoPackage->auth_;
	sg::GameDB                          = dbg_new MysqlDatabase();
	sg::GameDB->Initialize();
	sg::NetGroupMgr                      = AuthNetMaster::Get();
	sg::NetGroupMgr->Initialize();
	sg::NetGroup                        = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::MainId).Get<AuthNetGroup*>();
	sg::Server	                        = sg::NetGroup->GetAuthTcp();

	// COMMON INJECTION
	sg::CommonNetGroupMgr               = sg::NetGroupMgr;
	sg::CommonNetGroup                  = sg::NetGroup;
	sg::CommonServer                    = sg::Server;
	sg::CommonContents                  = &sg::Contents;
	sg::NetGroup_InterServ       = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::InterServerId).Get<NetGroup_InterServ*>();
	sg::InterServerClientTcp            = sg::NetGroup_InterServ ? sg::NetGroup_InterServ->GetInterServerClientTcp() : nullptr;
	sg::InterServerClientUdp            = sg::NetGroup_InterServ ? sg::NetGroup_InterServ->GetInterServerClientUdp() : nullptr;
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
