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

NS_CORE_BEGIN
::DataManager*				DataManager;
::MysqlDatabase*			GameDB;
::GameNetMaster*			NetMaster;
::GameNetGroup*				NetGroup;
::LogicServer*				Server;
::GameServerType_t			GameServerType;
::GameServerProcessInfo*	GameServerProcessInfo;
::RuntimeConfig*			RuntimeConfig;
::GameContents				Contents;
NS_CORE_END

void InitializeGameCore(GameServerType_t gameServerType) {
	Core::GameServerType			= gameServerType;
	Core::DataManager				= DataManager::Get();
	Core::CommonInfo				= Core::DataManager->getCommonInfo(1);
	Core::ServerProcessInfoPackage	= Core::DataManager->getServerProcessInfoPackage(1);						// 공통 라이브러리 주입
	Core::GameServerProcessInfo		= Core::ServerProcessInfoPackage->getGameServerProcessInfo(gameServerType);
	Core::ServerProcessInfo			= Core::GameServerProcessInfo;		// 공통 라이브러리 주입
	Core::GameDB					= dbg_new MysqlDatabase(Core::DataManager->getDatabaseInfo(DatabaseType::Game));
	Core::GameDB->Initialize(ServerProcessType::Game);
	Core::NetMaster					= GameNetMaster::Get();
	Core::NetMaster->SetProcessInfo(Core::ServerProcessInfo);
	Core::NetMaster->Initialize();
	Core::NetGroup					= Core::NetMaster->GetNetGroup(Const::NetGroup::MainId).Get<GameNetGroup*>();
	Core::InterServerClientNetGroup = Core::NetMaster->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	Core::Server					= Core::NetGroup->GetLogicTcp();
	Core::RuntimeConfig = RuntimeConfig::Get();
	Core::RuntimeConfig->Load();

	// BASE INJECTION
	if (Core::CLIThread)
		Core::CLIThread->SetListener(dbg_new CLIListener);

	Core::ServerProcessInfoPackage		= Core::DataManager->getServerProcessInfoPackage(1);		// 위에서 주입됨
	Core::CommonInfo					= Core::DataManager->getCommonInfo(1);						// 위에서 주입됨
	Core::CharCommon					= nullptr;													// 사용안함
	Core::ThreadPool					= dbg_new ThreadPool{ 2 };
	Core::Scheduler						= dbg_new Scheduler{ 2 };
	Core::RuntimeConfigBase				= Core::RuntimeConfig;

	// COMMON INJECTION
	Core::CommonNetMaster				= Core::NetMaster;
	Core::CommonNetGroup				= Core::NetGroup;
	Core::CommonServer					= Core::Server;
	Core::ServerProcessInfo				= Core::GameServerProcessInfo;								// 위에서 주입됨
	Core::InterServerClientNetGroup		= Core::NetMaster->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	Core::InterServerClientTcp			= Core::InterServerClientNetGroup ? Core::InterServerClientNetGroup->GetInterServerClientTcp() : nullptr;
	Core::InterServerClientUdp			= Core::InterServerClientNetGroup ? Core::InterServerClientNetGroup->GetInterServerClientUdp() : nullptr;
	Core::TimeManager					= TimeManager::Get();
	Core::RuntimeConfigCommon			= Core::RuntimeConfig;

	Core::Contents.Initialize();
}

void FinalizeGameCore() {
	Core::Contents.Finalize();

	JCORE_DELETE_SINGLETON_SAFE(Core::NetMaster);
	JCORE_DELETE_SINGLETON_SAFE(Core::DataManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::TimeManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::RuntimeConfig);
	JCORE_DELETE_SAFE(Core::GameDB);
}