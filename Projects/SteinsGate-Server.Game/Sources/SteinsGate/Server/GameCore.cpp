﻿/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Game.h"
#include "GameCore.h"
#include "GameCoreHeader.h"

USING_NS_JC;
USING_NS_JNET;

DataManager* CoreDataManager_v;
MysqlDatabase* CoreGameDB_v;
GameNetMaster* CoreNetMaster_v;
GameNetGroup* CoreNetGroup_v;
LogicServer* CoreServer_v;
GameTokenManager* CoreTokenManager_v;
GameServerType_t CoreGameServerType_v;
GameServerProcessInfo* CoreGameServerProcessInfo_v;

bool InitializeGameCore(GameServerType_t gameServerType) {
	CoreGameServerType_v = gameServerType;
	CoreDataManager_v = DataManager::Get();
	CoreCommonInfo_v = CoreDataManager_v->getCommonInfo(1);
	CoreServerProcessInfoPackage_v = CoreDataManager_v->getServerProcessInfoPackage(1);						// 공통 라이브러리 주입
	CoreGameServerProcessInfo_v = CoreServerProcessInfoPackage_v->getGameServerProcessInfo(gameServerType);
	CoreServerProcessInfo_v = CoreGameServerProcessInfo_v;		// 공통 라이브러리 주입

	if (CoreServerProcessInfo_v == nullptr) {
		return false;
	}

	CoreGameDB_v = dbg_new MysqlDatabase(CoreDataManager_v->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Game);
	CoreNetMaster_v = GameNetMaster::Get();
	CoreNetMaster_v->Initialize();
	CoreNetGroup_v = CoreNetMaster_v->GetNetGroup(NETGROUP_ID_MAIN).Get<GameNetGroup*>();
	CoreInterServerClientNetGroup_v = CoreNetMaster_v->GetNetGroup(NETGROUP_ID_INTERSERVER).Get<InterServerClientNetGroup*>();
	CoreServer_v = dynamic_cast<LogicServer*>(CoreNetGroup_v->GetServer());

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
	}

	return true;
}

void FinalizeGameCore() {
	CoreThreadPool_v->Join(ThreadPool::JoinStrategy::WaitAllTasks);
	CoreScheduler_v->Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

	JCORE_DELETE_SINGLETON_SAFE(CoreNetMaster_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreDataManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreTimeManager_v);
	JCORE_DELETE_SAFE(CoreThreadPool_v);
	JCORE_DELETE_SAFE(CoreScheduler_v);
	JCORE_DELETE_SAFE(CoreGameDB_v);
}