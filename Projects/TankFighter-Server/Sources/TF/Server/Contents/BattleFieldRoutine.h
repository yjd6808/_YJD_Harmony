/*
 * 작성자: 윤정도
 * 생성일: 10/21/2023 12:39:23 PM
 * =====================
 *
 */


#pragma once

class BattleFieldRoutine : public IUpdatable
{
public:
	bool Start(Room* room);
	void End(Room* room);
	void OnUpdate(const JCore::TimeSpan& elapsed) override;
private:
	JCore::NormalLock m_Sync;
	JCore::HashMap<int, Room*> m_hmRoomMapByAccessId;
};
