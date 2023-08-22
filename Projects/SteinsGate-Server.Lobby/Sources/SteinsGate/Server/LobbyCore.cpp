/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:34:29 AM [SteinsGate-Server.Auth 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Lobby.h"
#include "LobbyCore.h"
#include "LobbyCoreHeader.h"

#include <SteinsGate/Server/CLIListener.h>

USING_NS_JC;
USING_NS_JNET;

DataManager*		CoreDataManager_v;
MysqlDatabase*		CoreGameDB_v;
LobbyNetMaster*		CoreNetMaster_v;
LobbyNetGroup*		CoreNetGroup_v;
LobbyServer*		CoreServer_v;
LobbyTokenManager*	CoreTokenManager_v;
RuntimeConfig*		CoreRuntimeConfig_v;

void InitializeLobbyCore() {
	CoreDataManager_v				= DataManager::Get();
	CoreCommonInfo_v				= CoreDataManager_v->getCommonInfo(1);
	CoreServerProcessInfoPackage_v	= CoreDataManager_v->getServerProcessInfoPackage(1);				// 공통 라이브러리 주입
	CoreServerProcessInfo_v			= &CoreServerProcessInfoPackage_v->Lobby;							// 공통 라이브러리 주입
	CoreGameDB_v					= dbg_new MysqlDatabase(CoreDataManager_v->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Lobby);
	CoreNetMaster_v					= LobbyNetMaster::Get();
	CoreNetMaster_v->Initialize();
	CoreNetGroup_v					= CoreNetMaster_v->GetNetGroup(Const::NetGroup::MainId).Get<LobbyNetGroup*>();
	CoreInterServerClientNetGroup_v = CoreNetMaster_v->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	CoreServer_v					= CoreNetGroup_v->GetLobbyTcp();

	if (CoreCLIThread_v)	// 커몬코어에서 초기화되므로 체크
		CoreCLIThread_v->SetListener(dbg_new CLIListener);

	CoreRuntimeConfig_v = RuntimeConfig::Get();
	CoreRuntimeConfig_v->Load();

	// 공통 라이브러리 주입
	{
		CoreCommonNetMaster_v = CoreNetMaster_v;
		CoreCommonNetGroup_v = CoreNetGroup_v;
		CoreCommonServer_v = CoreServer_v;
		CoreInterServerClientTcp_v = CoreInterServerClientNetGroup_v->GetInterServerClientTcp();
		CoreInterServerClientUdp_v = CoreInterServerClientNetGroup_v->GetInterServerClientUdp();
		CoreTimeManager_v = TimeManager::Get();

		CoreThreadPool_v = dbg_new ThreadPool{ 2 };
		CoreScheduler_v = dbg_new Scheduler{ 2 };

		CoreRuntimeConfigBase_v = CoreRuntimeConfig_v;
		CoreRuntimeConfigCommon_v = CoreRuntimeConfig_v;
	}

}

void FinalizeLobbyCore() {
	CoreThreadPool_v->Join(ThreadPool::JoinStrategy::WaitAllTasks);
	CoreScheduler_v->Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

	JCORE_DELETE_SINGLETON_SAFE(CoreNetMaster_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreDataManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreTimeManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreRuntimeConfig_v);
	JCORE_DELETE_SAFE(CoreThreadPool_v);
	JCORE_DELETE_SAFE(CoreScheduler_v);
	JCORE_DELETE_SAFE(CoreGameDB_v);
}