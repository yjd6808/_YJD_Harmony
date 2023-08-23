/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 4:24:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCore.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Common/SgaElementInitializer.h>
#include <SteinsGate/Common/AudioPlayer.h>

#include <SteinsGate/Client/CLIListener.h>


// ===========================================================
//     슈타인즈 게이트 모든 세계션이 만나는 곳
//     사실.. 소괄호 작성하기 귀찮아서 이렇게 모아놓음
// ===========================================================

NS_CORE_BEGIN
::SteinsGateApp*		App;
::ClientInfo*			ClientInfo;
::Contents				Contents;
::DataManager*			DataManager;
::NetCore*				Net;
::RuntimeConfig*		RuntimeConfig;
NS_CORE_END

void InitializeClientCore() {
	Core::App						= (SteinsGateApp*)cocos2d::Application::getInstance();
	Core::DataManager				= DataManager::Get();
	Core::Net						= NetCore::Get();
	Core::RuntimeConfig				= RuntimeConfig::Get();
	Core::RuntimeConfigBase			= Core::RuntimeConfig;
	Core::ServerProcessInfoPackage  = Core::DataManager->getServerProcessInfoPackage(1);

	Core::Contents.Initialize();

	if (Core::RuntimeConfig)
		Core::RuntimeConfig->Load();

	if (Core::DataManager)
		Core::DataManager->loadAll();

	if (Core::Net)
		Core::Net->Initialize();

	if (Core::CLIThread)
		Core::CLIThread->SetListener(dbg_new CLIListener);
}

void FinalizeClientCore() {
	JCORE_DELETE_SINGLETON_SAFE(Core::Net);
	JCORE_DELETE_SINGLETON_SAFE(Core::DataManager);
	JCORE_DELETE_SINGLETON_SAFE(Core::RuntimeConfig);

	Core::Contents.Finalize();
	
}