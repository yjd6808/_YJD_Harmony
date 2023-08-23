/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCore.h"
#include "CenterCoreHeader.h"

#include <SteinsGate/Server/CLIListener.h>

USING_NS_JC;
USING_NS_JNET;

DataManager*		Core::DataManager;
MysqlDatabase*		CoreGameDB_v;
CenterNetMaster*	CoreNetMaster_v;
CenterNetGroup*		CoreNetGroup_v;
CenterServer*		CoreServer_v;
RuntimeConfig*		Core::RuntimeConfig;

void InitializeCenterCore() {
	Core::DataManager				= DataManager::Get();
	Core::CommonInfo				= Core::DataManager->getCommonInfo(1);
	Core::ServerProcessInfoPackage	= Core::DataManager->getServerProcessInfoPackage(1);
	CoreServerProcessInfo_v			= &Core::ServerProcessInfoPackage->Center;
	CoreGameDB_v					= dbg_new MysqlDatabase(Core::DataManager->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Center);
	CoreNetMaster_v					= CenterNetMaster::Get();
	CoreNetMaster_v->Initialize();
	CoreNetGroup_v					= CoreNetMaster_v->GetNetGroup(Const::NetGroup::MainId).Get<CenterNetGroup*>();
	CoreServer_v					= CoreNetGroup_v->GetCenterTcp();

	if (Core::CLIThread)
		Core::CLIThread->SetListener(dbg_new CLIListener);

	Core::RuntimeConfig = RuntimeConfig::Get();
	Core::RuntimeConfig->Load();
	
	// 공통 라이브러리 주입용
	{
		CoreInterServerClientNetGroup_v = CoreNetMaster_v->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
		CoreCommonNetMaster_v = CoreNetMaster_v;
		CoreCommonNetGroup_v = CoreNetGroup_v;
		CoreCommonServer_v = CoreServer_v;

		Core::RuntimeConfigBase = Core::RuntimeConfig;
		CoreRuntimeConfigCommon_v = Core::RuntimeConfig;
	}
}

void FinalizeCenterCore() {
	JCORE_DELETE_SINGLETON_SAFE(CoreNetMaster_v);
	JCORE_DELETE_SINGLETON_SAFE(Core::DataManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::RuntimeConfig);
	JCORE_DELETE_SAFE(CoreGameDB_v);
}