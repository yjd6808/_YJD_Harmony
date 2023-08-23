/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
 * =====================
 *
 */

#include "Game.h"
#include "GameCore.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Server/CLIListener.h>

USING_NS_JC;
USING_NS_JNET;

DataManager*			Core::DataManager;
MysqlDatabase*			CoreGameDB_v;
GameNetMaster*			CoreNetMaster_v;
GameNetGroup*			CoreNetGroup_v;
LogicServer*			CoreServer_v;
GameTokenManager*		CoreTokenManager_v;
GameServerType_t		CoreGameServerType_v;
GameServerProcessInfo*	CoreGameServerProcessInfo_v;
RuntimeConfig*			Core::RuntimeConfig;

bool InitializeGameCore(GameServerType_t gameServerType) {
	CoreGameServerType_v			= gameServerType;
	Core::DataManager				= DataManager::Get();
	Core::CommonInfo				= Core::DataManager->getCommonInfo(1);
	Core::ServerProcessInfoPackage	= Core::DataManager->getServerProcessInfoPackage(1);						// 공통 라이브러리 주입
	CoreGameServerProcessInfo_v		= Core::ServerProcessInfoPackage->getGameServerProcessInfo(gameServerType);
	CoreServerProcessInfo_v			= CoreGameServerProcessInfo_v;		// 공통 라이브러리 주입

	if (CoreServerProcessInfo_v == nullptr) {
		return false;
	}

	CoreGameDB_v					= dbg_new MysqlDatabase(Core::DataManager->getDatabaseInfo(DatabaseType::Game));
	CoreGameDB_v->Initialize(ServerProcessType::Game);
	CoreNetMaster_v					= GameNetMaster::Get();
	CoreNetMaster_v->Initialize();
	CoreNetGroup_v					= CoreNetMaster_v->GetNetGroup(Const::NetGroup::MainId).Get<GameNetGroup*>();
	CoreInterServerClientNetGroup_v = CoreNetMaster_v->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	CoreServer_v					= CoreNetGroup_v->GetLogicTcp();

	if (Core::CLIThread)	// 커몬코어에서 초기화되므로 체크
		Core::CLIThread->SetListener(dbg_new CLIListener);

	Core::RuntimeConfig = RuntimeConfig::Get();
	Core::RuntimeConfig->Load();

	// 공통 라이브러리 주입
	{
		CoreCommonNetMaster_v = CoreNetMaster_v;
		CoreCommonNetGroup_v = CoreNetGroup_v;
		CoreCommonServer_v = CoreServer_v;
		CoreInterServerClientTcp_v = CoreInterServerClientNetGroup_v->GetInterServerClientTcp();
		CoreInterServerClientUdp_v = CoreInterServerClientNetGroup_v->GetInterServerClientUdp();
		Core::Contents.TimeManager = TimeManager::Get();

		Core::ThreadPool = dbg_new ThreadPool{ 2 };
		Core::Scheduler = dbg_new Scheduler{ 2 };

		Core::RuntimeConfigBase = Core::RuntimeConfig;
		CoreRuntimeConfigCommon_v = Core::RuntimeConfig;
	}

	return true;
}

void FinalizeGameCore() {
	Core::ThreadPool->Join(ThreadPool::JoinStrategy::WaitAllTasks);
	Core::Scheduler->Join(Scheduler::JoinStrategy::WaitOnlyRunningTask);

	JCORE_DELETE_SINGLETON_SAFE(CoreNetMaster_v);
	JCORE_DELETE_SINGLETON_SAFE(Core::DataManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::Contents.TimeManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::RuntimeConfig);
	JCORE_DELETE_SAFE(Core::ThreadPool);
	JCORE_DELETE_SAFE(Core::Scheduler);
	JCORE_DELETE_SAFE(CoreGameDB_v);
}