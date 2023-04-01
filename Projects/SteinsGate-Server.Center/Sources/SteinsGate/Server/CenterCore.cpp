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

void InitializeCenterCore() {
	CoreDataManager_v = DataManager::get();
	CoreCommon_v = CoreDataManager_v->getCommonInfo(1);
	CoreServer_v = CoreDataManager_v->getServerInfo(1);
	CoreGameDB_v = dbg_new MysqlDatabase(CoreDataManager_v->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Center);
	CoreNetMaster_v = CenterNetMaster::Get();
	CoreNetMaster_v->Initialize();
}

void FinalizeCenterCore() {
	DeleteSafe(CoreNetMaster_v);
	DeleteSafe(CoreGameDB_v);
	DeleteSafe(CoreDataManager_v);
}