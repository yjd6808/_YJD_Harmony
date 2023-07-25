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


// ===========================================================
//     슈타인즈 게이트 모든 세계션이 만나는 곳
//     사실.. 소괄호 작성하기 귀찮아서 이렇게 모아놓음
// ===========================================================

SGApplication*			CoreApp_v;
ClientInfo*				CoreClientInfo_v;
HostPlayer*				CorePlayer_v;
DataManager*			CoreDataManager_v;
UIManager*				CoreUIManager_v;
WorldScene*				CoreWorld_v;
Inven*					CoreInven_v;
FontPackage*			CoreFont_v;
ActorListenerManager*	CoreActorListenerManager_v;
ActorBox*				CoreActorBox_v;
ImagePackManager*		CorePackManager_v;
Global*					CoreGlobal_v;
PopupManager*			CorePopupManager_v;
TimeManager*			CoreTimeManager_v;
NetCore*				CoreNet_v;

void InitializeClientCore() {
	CoreApp_v					= (SGApplication*)cocos2d::Application::getInstance();
	CoreGlobal_v				= Global::Get();
	CorePackManager_v			= ImagePackManager::Get();
	CorePackManager_v->loadAllPackages();
	CoreFont_v					= FontPackage::Get();
	CoreDataManager_v			= DataManager::Get();
	CoreUIManager_v				= UIManager::Get();
	CorePlayer_v				= HostPlayer::Get();
	CoreInven_v					= Inven::Get();
	CoreWorld_v					= WorldScene::get();
	CoreActorListenerManager_v  = ActorListenerManager::Get();
	CoreActorBox_v				= ActorBox::Get();
	CorePopupManager_v			= PopupManager::Get();
	CoreTimeManager_v			= TimeManager::Get();
	CoreNet_v					= NetCore::Get();
	CoreServerProcessInfoPackage_v = CoreDataManager_v->getServerProcessInfoPackage(1);

	CoreGlobal_v->init();
	CoreFont_v->init();
	CoreDataManager_v->loadAll();
	CoreUIManager_v->init();
	CoreInven_v->init();
	CoreActorListenerManager_v->init();
	CoreNet_v->Initialize();
	
}

void FinalizeClientCore() {

	JCORE_DELETE_SINGLETON_SAFE(CoreNet_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreTimeManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CorePopupManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreUIManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CorePlayer_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreDataManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreInven_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreFont_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreActorListenerManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreActorBox_v);
	JCORE_DELETE_SINGLETON_SAFE(CorePackManager_v);
	JCORE_DELETE_SINGLETON_SAFE(CoreGlobal_v);
	
	JCORE_MAKE_NULL(CoreWorld_v);	// 월드는 코코스에서 알아서 제거해줌
}