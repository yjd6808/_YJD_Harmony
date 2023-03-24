/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCore.h"
#include "AuthCoreHeader.h"

USING_NS_JC;
USING_NS_JNET;

DataManager* CoreDataManager_v;
MysqlDatabase* CoreGameDB_v;
AuthNetMaster* CoreNetMaster_v;

void InitializeAuthCore() {
	CoreDataManager_v = DataManager::get();
	CoreCommon_v = CoreDataManager_v->getCommonInfo(1);
	CoreServer_v = CoreDataManager_v->getServerInfo(1);
	CoreGameDB_v = dbg_new MysqlDatabase(CoreDataManager_v->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Auth);
	CoreNetMaster_v = AuthNetMaster::Get();
	CoreNetMaster_v->Initialize();
}

void FinalizeAuthCore() {
	DeleteSafe(CoreNetMaster_v);
	DeleteSafe(CoreGameDB_v);
	DeleteSafe(CoreDataManager_v);
}