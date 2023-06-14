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
AuthNetGroup* CoreNetGroup_v;
AuthServer* CoreServer_v;
AuthTokenManager* CoreTokenManager_v;

void InitializeAuthCore() {
	CoreDataManager_v = DataManager::Get();
	CoreCommonInfo_v = CoreDataManager_v->getCommonInfo(1);
	CoreServerProcessInfo_v = CoreDataManager_v->getServerProcessInfo(1);
	CoreGameDB_v = dbg_new MysqlDatabase(CoreDataManager_v->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Auth);
	CoreNetMaster_v = AuthNetMaster::Get();
	CoreNetMaster_v->Initialize();
	CoreNetGroup_v = CoreNetMaster_v->GetNetGroup(1).Get<AuthNetGroup*>();
	CoreServer_v = dynamic_cast<AuthServer*>(CoreNetGroup_v->GetServer());
	CoreTokenManager_v = AuthTokenManager::Get();

	// 공통 라이브러리 주입용
	{
		CoreCommonNetMaster_v = CoreNetMaster_v;
		CoreCommonNetGroup_v = CoreNetGroup_v;
		CoreCommonServer_v = CoreServer_v;
		CoreCenterClient_v = CoreNetGroup_v->GetCenterClient();
		CoreInterServerClient_v = CoreNetGroup_v->GetInterServerClient();

		CoreThreadPool_v = dbg_new ThreadPool{ 2 };
		CoreScheduler_v = dbg_new Scheduler{ 2 };
	}

}

void FinalizeAuthCore() {
	CoreThreadPool_v->Join(ThreadPool::JoinStrategy::WaitAllTasks);
	CoreScheduler_v->Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

	JCORE_DELETE_SINGLETON_SAFE(CoreNetMaster_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreDataManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreTokenManager_v);
	JCORE_DELETE_SAFE(CoreThreadPool_v);
	JCORE_DELETE_SAFE(CoreScheduler_v);
	JCORE_DELETE_SAFE(CoreGameDB_v);
}