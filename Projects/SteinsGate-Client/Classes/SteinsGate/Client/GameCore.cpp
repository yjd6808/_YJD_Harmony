/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 4:24:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCore.h"

#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGUIManager.h>
#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGInven.h>
#include <SteinsGate/Client/SGWorldScene.h>
#include <SteinsGate/Client/SGFontPackage.h>
#include <SteinsGate/Client/SGActorListenerManager.h>
#include <SteinsGate/Client/SGActorBox.h>
#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGImagePackManager.h>

// ===========================================================
//     슈타인즈 게이트 모든 세계션이 만나는 곳
// ===========================================================

SGClientInfo*				CoreInfo_v;
SGHostPlayer*				CorePlayer_v;
SGDataManager*				CoreDataManager_v;
SGUIManager*				CoreUIManager_v;
SGWorldScene*				CoreWorld_v;
SGInven*					CoreInven_v;
SGFontPackage*				CoreFont_v;
SGActorListenerManager*		CoreActorListenerManager_v;
SGActorBox*					CoreActorBox_v;
SGImagePackManager*			CorePackManager_v;
SGGlobal*					CoreGlobal_v;

void InitializeClientCore() {
	CoreUIManager_v				= SGUIManager::get();
	CorePlayer_v				= SGHostPlayer::get();
	CoreDataManager_v			= SGDataManager::get();
	CoreInven_v					= SGInven::get();
	CoreWorld_v					= SGWorldScene::get();
	CoreFont_v					= SGFontPackage::get();
	CoreActorListenerManager_v  = SGActorListenerManager::get();
	CoreActorBox_v				= SGActorBox::get();
	CorePackManager_v			= SGImagePackManager::get();
	CoreGlobal_v				= SGGlobal::get();
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