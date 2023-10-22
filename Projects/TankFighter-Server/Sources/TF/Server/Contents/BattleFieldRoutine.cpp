/*
 * 작성자: 윤정도
 * 생성일: 10/21/2023 12:39:36 PM
 * =====================
 *
 */


#include "Pch.h"
#include "BattleFieldRoutine.h"

#include <TF/Common/Command.h>
#include <TF/Server/Const.h>

USING_NS_JC;
USING_NS_JNET;

bool BattleFieldRoutine::Start(Room* room) {
	{
		JCORE_LOCK_GUARD(m_Sync);
		if (m_hmRoomMapByAccessId.Insert(room->GetAccessId(), room) == false) {
			return false;
		}
	}

	room->OnBattleBegin();
	return true;
}

void BattleFieldRoutine::End(Room* room) {
	{
		JCORE_LOCK_GUARD(m_Sync);
		m_hmRoomMapByAccessId.Remove(room->GetAccessId());
	}

	room->OnBattleEnd();
}

void BattleFieldRoutine::OnUpdate(const TimeSpan& elapsed) {
	static Vector<Room*> s_vUpdatableRooms;

	s_vUpdatableRooms.Clear();
	{
		JCORE_LOCK_GUARD(m_Sync);
		m_hmRoomMapByAccessId.Values().Extension().ForEach([&](Room* room) { s_vUpdatableRooms.PushBack(room); });
	}

	for (int i = 0; i < s_vUpdatableRooms.Size(); ++i) {
		s_vUpdatableRooms[i]->OnUpdateBattle(elapsed);
	}
}
