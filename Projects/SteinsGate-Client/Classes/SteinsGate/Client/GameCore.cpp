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

	CoreGlobal_v->init();
	CorePackManager_v->loadAllPackages();
	CoreFont_v->init();
	CoreDataManager_v->loadAll();
	CoreUIManager_v->init();
	CoreInven_v->init();
	CoreActorListenerManager_v->init();
	CoreNet_v->Initialize();
}

void FinalizeClientCore() {

	DeleteSingletonSafe(CoreNet_v);
	DeleteSingletonSafe(CoreTimeManager_v);
	DeleteSingletonSafe(CorePopupManager_v);
	DeleteSingletonSafe(CoreUIManager_v);
	DeleteSingletonSafe(CorePlayer_v);
	DeleteSingletonSafe(CoreDataManager_v);
	DeleteSingletonSafe(CoreInven_v);
	DeleteSingletonSafe(CoreFont_v);
	DeleteSingletonSafe(CoreActorListenerManager_v);
	DeleteSingletonSafe(CoreActorBox_v);
	DeleteSingletonSafe(CorePackManager_v);
	DeleteSingletonSafe(CoreGlobal_v);
	
	Null(CoreWorld_v);	// 월드는 코코스에서 알아서 제거해줌
}