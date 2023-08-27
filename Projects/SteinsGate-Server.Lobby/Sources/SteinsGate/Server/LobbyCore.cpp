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

NS_CORE_BEGIN
::DataManager*		DataManager;
::MysqlDatabase*	GameDB;
::LobbyNetMaster*	NetMaster;
::LobbyNetGroup*	NetGroup;
::LobbyServer*		Server;
::RuntimeConfig*	RuntimeConfig;
::LobbyContents		Contents;
NS_CORE_END

void InitializeLobbyCore() {
	Core::DataManager				= DataManager::Get();
	Core::CommonInfo				= Core::DataManager->getCommonInfo(1);
	Core::ServerProcessInfoPackage	= Core::DataManager->getServerProcessInfoPackage(1);				// 공통 라이브러리 주입
	Core::ServerProcessInfo			= &Core::ServerProcessInfoPackage->Lobby;							// 공통 라이브러리 주입
	Core::GameDB					= dbg_new MysqlDatabase(Core::DataManager->getDatabaseInfo(DatabaseType::Game));
	Core::GameDB->Initialize(ServerProcessType::Lobby);
	Core::NetMaster					= LobbyNetMaster::Get();
	Core::NetMaster->SetProcessInfo(Core::ServerProcessInfo);
	Core::NetMaster->Initialize();
	Core::NetGroup					= Core::NetMaster->GetNetGroup(Const::NetGroup::MainId).Get<LobbyNetGroup*>();
	Core::Server					= Core::NetGroup->GetLobbyTcp();
	Core::RuntimeConfig				= RuntimeConfig::Get();
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
	Core::CommonRuntimeConfig			= Core::RuntimeConfig;
	Core::CommonContents				= &Core::Contents;
	Core::InterServerClientNetGroup		= Core::NetMaster->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	Core::InterServerClientTcp			= Core::InterServerClientNetGroup ? Core::InterServerClientNetGroup->GetInterServerClientTcp() : nullptr;
	Core::InterServerClientUdp			= Core::InterServerClientNetGroup ? Core::InterServerClientNetGroup->GetInterServerClientUdp() : nullptr;
	Core::ServerProcessInfo				= &Core::ServerProcessInfoPackage->Lobby;					// 위에서 주입됨
	Core::TimeManager					= TimeManager::Get();
	

	Core::Contents.Initialize();

}

void FinalizeLobbyCore() {
	Core::Contents.Finalize();

	JCORE_DELETE_SAFE(Core::GameDB);
	JCORE_DELETE_SINGLETON_SAFE(Core::TimeManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::RuntimeConfig);
	JCORE_DELETE_SINGLETON_SAFE(Core::NetMaster);
	JCORE_DELETE_SINGLETON_SAFE(Core::DataManager);
	
}