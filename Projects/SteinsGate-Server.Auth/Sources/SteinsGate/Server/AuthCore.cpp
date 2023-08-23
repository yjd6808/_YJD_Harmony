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

NS_CORE_BEGIN
::DataManager*			DataManager;
::MysqlDatabase*		GameDB;
::AuthNetMaster*		NetMaster;
::AuthNetGroup*			NetGroup;
::AuthServer*			Server;
::AuthContents			Contents;
::RuntimeConfig*		RuntimeConfig;
NS_CORE_END

void InitializeAuthCore() {
	Core::DataManager						= DataManager::Get();
	Core::CommonInfo						= Core::DataManager->getCommonInfo(1);
	Core::ServerProcessInfoPackage			= Core::DataManager->getServerProcessInfoPackage(1);
	Core::ServerProcessInfo					= &Core::ServerProcessInfoPackage->Auth;
	Core::GameDB							= dbg_new MysqlDatabase(Core::DataManager->getDatabaseInfo(DatabaseType::Game));
	Core::GameDB->Initialize(ServerProcessType::Auth);
	Core::NetMaster							= AuthNetMaster::Get();
	Core::NetMaster->Initialize();
	Core::NetGroup							= Core::NetMaster->GetNetGroup(Const::NetGroup::MainId).Get<AuthNetGroup*>();
	Core::Server						    = Core::NetGroup->GetAuthTcp();
	Core::RuntimeConfig						= RuntimeConfig::Get();
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
	Core::ServerProcessInfo				= &Core::ServerProcessInfoPackage->Auth;
	Core::InterServerClientNetGroup		= Core::NetMaster->GetNetGroup(Const::NetGroup::InterServerId).Get<InterServerClientNetGroup*>();
	Core::InterServerClientTcp			= Core::InterServerClientNetGroup ? Core::InterServerClientNetGroup->GetInterServerClientTcp() : nullptr;
	Core::InterServerClientUdp			= Core::InterServerClientNetGroup ? Core::InterServerClientNetGroup->GetInterServerClientUdp() : nullptr;
	Core::TimeManager					= TimeManager::Get();
	Core::RuntimeConfigCommon			= Core::RuntimeConfig;

	Core::Contents.Initialize();
}

void FinalizeAuthCore() {
	Core::Contents.Finalize();

	JCORE_DELETE_SINGLETON_SAFE(Core::DataManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::TimeManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::RuntimeConfig);
	JCORE_DELETE_SINGLETON_SAFE(Core::NetMaster);
	JCORE_DELETE_SAFE(Core::GameDB);
}