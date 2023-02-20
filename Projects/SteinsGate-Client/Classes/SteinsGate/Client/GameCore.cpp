/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 4:24:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCore.h"
#include "GameCoreHeader.h"


// ===========================================================
//     슈타인즈 게이트 모든 세계션이 만나는 곳
//     사실.. 소괄호 작성하기 귀찮아서 이렇게 모아놓음
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
	CorePackManager_v			= SGImagePackManager::get();
	CoreUIManager_v				= SGUIManager::get();
	CorePlayer_v				= SGHostPlayer::get();
	CoreDataManager_v			= SGDataManager::get();
	CoreInven_v					= SGInven::get();
	CoreWorld_v					= SGWorldScene::get();
	CoreFont_v					= SGFontPackage::get();
	CoreActorListenerManager_v  = SGActorListenerManager::get();
	CoreActorBox_v				= SGActorBox::get();
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