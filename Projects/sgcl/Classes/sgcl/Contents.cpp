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
	Player = HostPlayer::Get();
	Inven = Inven::Get();
	ActorListenerManager = ActorListenerManager::Get();
	ActorManager = ActorManager::Get();
	PopupManager = PopupManager::Get();
	TimeManager = TimeManager::Get();

	g_cUIMgr.Init();
	Inven->Init();
	ActorListenerManager->Init();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Contents::Finalize()
{
	JC_DELETE_SINGLETON_SAFE(TimeManager);
	JC_DELETE_SINGLETON_SAFE(PopupManager);

	g_cUIMgr.Free();

	JC_DELETE_SINGLETON_SAFE(Player);	// TODO: 수정 필요함. 코코스 오브젝트라서 여기서 해제하면 원래 안됨. removeAllChildren 전에 해제되어야한다. 총쏘고 나서 사라진 후 다시 쏠 때 강종해보면 오류 발생하는것알 수 있음. 코코스에서 객체를 삭제를 지멋대로 해버림
	JC_DELETE_SINGLETON_SAFE(Inven);
	JC_DELETE_SINGLETON_SAFE(ActorListenerManager);
	JC_DELETE_SINGLETON_SAFE(ActorManager);

	// JC_MAKE_NULL(World); // 월드는 코코스에서 알아서 제거해줌
}
