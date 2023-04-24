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

ClientInfo*				CoreClient_v;
HostPlayer*				CorePlayer_v;
DataManager*				CoreDataManager_v;
UIManager*				CoreUIManager_v;
WorldScene*				CoreWorld_v;
Inven*					CoreInven_v;
FontPackage*				CoreFont_v;
ActorListenerManager*		CoreActorListenerManager_v;
ActorBox*					CoreActorBox_v;
ImagePackManager*			CorePackManager_v;
SGGlobal*					CoreGlobal_v;

void InitializeClientCore() {
	CoreGlobal_v				= SGGlobal::get();
	CorePackManager_v			= ImagePackManager::get();
	CoreFont_v					= FontPackage::get();
	CoreDataManager_v			= DataManager::get();
	CoreUIManager_v				= UIManager::get();
	CorePlayer_v				= HostPlayer::get();
	CoreInven_v					= Inven::get();
	CoreWorld_v					= WorldScene::get();
	CoreActorListenerManager_v  = ActorListenerManager::get();
	CoreActorBox_v				= ActorBox::get();

	CoreGlobal_v->init();
	CorePackManager_v->loadAllPackages();
	CoreFont_v->init();
	CoreDataManager_v->loadAll();
	CoreUIManager_v->init();
	CoreInven_v->init();
	CoreActorListenerManager_v->init();
}

void FinalizeClientCore() {
	
	DeleteSafe(CoreUIManager_v);
	DeleteSafe(CorePlayer_v);
	DeleteSafe(CoreDataManager_v);
	DeleteSafe(CoreInven_v);
	DeleteSafe(CoreFont_v);
	DeleteSafe(CoreActorListenerManager_v);
	DeleteSafe(CoreActorBox_v);
	DeleteSafe(CorePackManager_v);
	DeleteSafe(CoreGlobal_v);
	Null(CoreWorld_v);
}