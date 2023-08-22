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

DataManager*		CoreDataManager_v;
MysqlDatabase*		CoreGameDB_v;
CenterNetMaster*	CoreNetMaster_v;
CenterNetGroup*		CoreNetGroup_v;
CenterServer*		CoreServer_v;
RuntimeConfig*		CoreRuntimeConfig_v;

void InitializeCenterCore() {
	CoreDataManager_v				= DataManager::Get();
	CoreCommonInfo_v				= CoreDataManager_v->getCommonInfo(1);
	CoreServerProcessInfoPackage_v	= CoreDataManager_v->getServerProcessInfoPackage(1);
	CoreServerProcessInfo_v			= &CoreServerProcessInfoPackage_v->Center;
	CoreGameDB_v					= dbg_new MysqlDatabase(CoreDataManager_v->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Center);
	CoreNetMaster_v					= CenterNetMaster::Get();
	CoreNetMaster_v->Initialize();
	CoreNetGroup_v					= CoreNetMaster_v->GetNetGroup(Const::NetGroup::MainId).Get<CenterNetGroup*>();
	CoreServer_v					= CoreNetGroup_v->GetCenterTcp();

	if (CoreCLIThread_v)
		CoreCLIThread_v->SetListener(dbg_new CLIListener);

	CoreRuntimeConfig_v = RuntimeConfig::Get();
	CoreRuntimeConfig_v->Load();
	
	// 공통 라이브러리 주입용
	{
		CoreInterServerClientNetGroup_v = CoreNetMaster_v->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
		CoreCommonNetMaster_v = CoreNetMaster_v;
		CoreCommonNetGroup_v = CoreNetGroup_v;
		CoreCommonServer_v = CoreServer_v;

		CoreRuntimeConfigBase_v = CoreRuntimeConfig_v;
		CoreRuntimeConfigCommon_v = CoreRuntimeConfig_v;
	}
}

void FinalizeCenterCore() {
	JCORE_DELETE_SINGLETON_SAFE(CoreNetMaster_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreDataManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreRuntimeConfig_v);
	JCORE_DELETE_SAFE(CoreGameDB_v);
}