/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 1:24:06 PM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "Contents.h"

#include "UIManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
void Contents::Initialize()
{
	Global = Global::Get();
	Player = HostPlayer::Get();
	Inven = Inven::Get();
	ActorListenerManager = ActorListenerManager::Get();
	ActorManager = ActorManager::Get();
	PopupManager = PopupManager::Get();
	TimeManager = TimeManager::Get();

	Global->init();
	g_cUIMgr.Init();
	Inven->Init();
	ActorListenerManager->Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Contents::Finalize()
{
	JCORE_DELETE_SINGLETON_SAFE(TimeManager);
	JCORE_DELETE_SINGLETON_SAFE(PopupManager);

	g_cUIMgr.Free();

	JCORE_DELETE_SINGLETON_SAFE(Player);
	JCORE_DELETE_SINGLETON_SAFE(Inven);
	JCORE_DELETE_SINGLETON_SAFE(ActorListenerManager);
	JCORE_DELETE_SINGLETON_SAFE(ActorManager);
	JCORE_DELETE_SINGLETON_SAFE(Global);

	// JCORE_MAKE_NULL(World); // 월드는 코코스에서 알아서 제거해줌
}
