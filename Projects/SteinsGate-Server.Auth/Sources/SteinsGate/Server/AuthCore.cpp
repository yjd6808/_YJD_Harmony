/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 2:18:17 PM
 * =====================
 *
 */

#include "Auth.h"
#include "AuthCore.h"
#include "AuthCoreHeader.h"

#include <SteinsGate/Server/CLIListener.h>

USING_NS_JC;
USING_NS_JNET;

DataManager*				Core::DataManager;
MysqlDatabase*				CoreGameDB_v;
AuthNetMaster*				CoreNetMaster_v;
AuthNetGroup*				CoreNetGroup_v;
AuthServer*					CoreServer_v;
AuthenticationManager*		CoreTokenManager_v;
RuntimeConfig*				Core::RuntimeConfig;

void InitializeAuthCore() {
	Core::DataManager						= DataManager::Get();
	Core::CommonInfo						= Core::DataManager->getCommonInfo(1);
	Core::ServerProcessInfoPackage			= Core::DataManager->getServerProcessInfoPackage(1);				// 공통 라이브러리 주입
	CoreServerProcessInfo_v					= &Core::ServerProcessInfoPackage->Auth;							// 공통 라이브러리 주입
	CoreGameDB_v							= dbg_new MysqlDatabase(Core::DataManager->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Auth);
	CoreNetMaster_v							= AuthNetMaster::Get();
	CoreNetMaster_v->Initialize();
	CoreNetGroup_v							= CoreNetMaster_v->GetNetGroup(Const::NetGroup::MainId).Get<AuthNetGroup*>();
	CoreInterServerClientNetGroup_v			= CoreNetMaster_v->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	CoreServer_v						    = CoreNetGroup_v->GetAuthTcp();
	CoreTokenManager_v						= AuthenticationManager::Get();

	if (Core::CLIThread)	// 커몬코어에서 초기화되므로 체크
		Core::CLIThread->SetListener(dbg_new CLIListener);

	Core::RuntimeConfig = RuntimeConfig::Get();
	Core::RuntimeConfig->Load();

	// 공통 라이브러리 주입
	{
		CoreCommonNetMaster_v = CoreNetMaster_v;
		CoreCommonNetGroup_v = CoreNetGroup_v;
		CoreCommonServer_v = CoreServer_v;

		if (CoreInterServerClientNetGroup_v) {
			CoreInterServerClientTcp_v = CoreInterServerClientNetGroup_v->GetInterServerClientTcp();
			CoreInterServerClientUdp_v = CoreInterServerClientNetGroup_v->GetInterServerClientUdp();
		}

		Core::Contents.TimeManager = TimeManager::Get();

		Core::ThreadPool = dbg_new ThreadPool{ 2 };
		Core::Scheduler = dbg_new Scheduler{ 2 };

		Core::RuntimeConfigBase = Core::RuntimeConfig;
		CoreRuntimeConfigCommon_v = Core::RuntimeConfig;
	}

}

void FinalizeAuthCore() {
	Core::ThreadPool->Join(ThreadPool::JoinStrategy::WaitAllTasks);
	Core::Scheduler->Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

	JCORE_DELETE_SINGLETON_SAFE(Core::DataManager);
	JCORE_DELETE_SINGLETON_SAFE(CoreTokenManager_v);
	JCORE_DELETE_SINGLETON_SAFE(Core::Contents.TimeManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::RuntimeConfig);
	JCORE_DELETE_SINGLETON_SAFE(CoreNetMaster_v);
	JCORE_DELETE_SAFE(Core::ThreadPool);
	JCORE_DELETE_SAFE(Core::Scheduler);
	JCORE_DELETE_SAFE(CoreGameDB_v);
}