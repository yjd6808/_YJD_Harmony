/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCore.h"
#include "CenterCoreHeader.h"

USING_NS_JC;
USING_NS_JNET;

DataManager* CoreDataManager_v;
MysqlDatabase* CoreGameDB_v;
CenterNetMaster* CoreNetMaster_v;
CenterNetGroup* CoreNetGroup_v;
CenterServer* CoreServer_v;

void InitializeCenterCore() {
	CoreDataManager_v = DataManager::Get();
	CoreCommonInfo_v = CoreDataManager_v->getCommonInfo(1);
	CoreServerProcessInfo_v = CoreDataManager_v->getServerProcessInfo(1);
	CoreGameDB_v = dbg_new MysqlDatabase(CoreDataManager_v->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Center);
	CoreNetMaster_v = CenterNetMaster::Get();
	CoreNetMaster_v->Initialize();
	CoreNetGroup_v = CoreNetMaster_v->GetNetGroup(1).Get<CenterNetGroup*>();
	CoreServer_v = dynamic_cast<CenterServer*>(CoreNetGroup_v->GetServer());
	
	// 공통 라이브러리 주입용
	{
		CoreCommonNetMaster_v = CoreNetMaster_v;
		CoreCommonNetGroup_v = CoreNetGroup_v;
		CoreCommonServer_v = CoreServer_v;
		CoreCenterClient_v = nullptr;	// 중앙서버는 사용안하므로
		CoreInterServerClient_v = CoreNetGroup_v->GetInterServerClient();
	}
}

void FinalizeCenterCore() {
	DeleteSingletonSafe(CoreNetMaster_v);
	DeleteSingletonSafe(CoreDataManager_v);
	DeleteSafe(CoreGameDB_v);
}