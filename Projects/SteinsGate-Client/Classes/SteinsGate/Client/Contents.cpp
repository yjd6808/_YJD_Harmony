/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:24:06 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "Contents.h"


void Contents::Initialize() {
	Global					= Global::Get();
	PackManager				= ImagePackManager::Get();
	PackManager->loadAllPackages();
	FontManager				= FontManager::Get();
	UIManager				= UIManager::Get();
	Player					= HostPlayer::Get();
	Inven					= Inven::Get();
	World					= WorldScene::get();
	ActorListenerManager	= ActorListenerManager::Get();
	ActorManager			= ActorManager::Get();
	PopupManager			= PopupManager::Get();
	TimeManager				= TimeManager::Get();

	Global->init();
	FontManager->init();
	UIManager->init();
	Inven->init();
	ActorListenerManager->init();
}

void Contents::Finalize() {
	JCORE_DELETE_SINGLETON_SAFE(TimeManager);
	JCORE_DELETE_SINGLETON_SAFE(PopupManager);
	JCORE_DELETE_SINGLETON_SAFE(UIManager);
	JCORE_DELETE_SINGLETON_SAFE(Player);
	JCORE_DELETE_SINGLETON_SAFE(Inven);
	JCORE_DELETE_SINGLETON_SAFE(FontManager);
	JCORE_DELETE_SINGLETON_SAFE(ActorListenerManager);
	JCORE_DELETE_SINGLETON_SAFE(ActorManager);
	JCORE_DELETE_SINGLETON_SAFE(PackManager);
	JCORE_DELETE_SINGLETON_SAFE(Global);
	JCORE_MAKE_NULL(World);	// 월드는 코코스에서 알아서 제거해줌
}
