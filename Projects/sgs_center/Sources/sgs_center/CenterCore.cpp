/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Core.h"
#include "CenterCore.h"
#include "CenterCoreHeader.h"

#include <sgs_center/CLIListener.h>

USING_NS_JC;
USING_NS_JNET;

NS_SG_BEGIN
::DataManager* DataManager;
::MysqlDatabase* GameDB;
::CenterNetMaster* NetGroupMgr;
::CenterNetGroup* NetGroup;
::CenterServer* Server;
::RuntimeConfig* RuntimeConfig;
::CenterContents	Contents;
NS_SG_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeCenterCore() 
{
	sg::DataManager = DataManager::Get();
	sg::ServerProcessInfoPackage = sg::DataManager->GetServerProcessInfoPackage(1);
	sg::ServerProcessInfo = &sg::ServerProcessInfoPackage->center_;
	sg::GameDB = dbg_new MysqlDatabase(sg::DataManager->GetDatabaseInfo(DatabaseType::Game));
	sg::GameDB->Initialize(ServerProcessType::Center);
	sg::NetGroupMgr = CenterNetMaster::Get();
	sg::NetGroupMgr->Initialize();
	sg::NetGroup = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::MainId).Get<CenterNetGroup*>();
	sg::Server = sg::NetGroup->GetCenterTcp();
	sg::RuntimeConfig = RuntimeConfig::Get();
	sg::RuntimeConfig->Load();

	// BASE INJECTION
	if (sg::CLIThread)
		sg::CLIThread->SetListener(dbg_new CLIListener);

	sg::ServerProcessInfoPackage = sg::DataManager->GetServerProcessInfoPackage(1);		// 위에서 주입됨
	sg::CharCommon = nullptr;													// 사용안함
	sg::ThreadPool = dbg_new ThreadPool{ 2 };
	sg::Scheduler = dbg_new Scheduler{ 2 };
	sg::RuntimeConfig = sg::RuntimeConfig;

	// COMMON INJECTION
	sg::CommonNetGroupMgr = sg::NetGroupMgr;
	sg::CommonNetGroup = sg::NetGroup;
	sg::CommonServer = sg::Server;
	sg::CommonRuntimeConfig = sg::RuntimeConfig;
	sg::CommonContents = &sg::Contents;
	sg::InterServerClientNetGroup = sg::NetGroupMgr->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	sg::InterServerClientTcp = sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientTcp() : nullptr;
	sg::InterServerClientUdp = sg::InterServerClientNetGroup ? sg::InterServerClientNetGroup->GetInterServerClientUdp() : nullptr;
	sg::ServerProcessInfo = &sg::ServerProcessInfoPackage->center_;
	sg::TimeManager = TimeManager::Get();

	sg::Contents.Initialize();
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeCenterCore() 
{
	sg::Contents.Finalize();

	JC_DELETE_SAFE(sg::GameDB);
	JC_DELETE_SINGLETON_SAFE(sg::TimeManager);
	JC_DELETE_SINGLETON_SAFE(sg::RuntimeConfig);
	JC_DELETE_SINGLETON_SAFE(sg::NetGroupMgr);
	JC_DELETE_SINGLETON_SAFE(sg::DataManager);

}