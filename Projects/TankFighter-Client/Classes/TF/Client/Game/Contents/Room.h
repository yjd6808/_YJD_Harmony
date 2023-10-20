/*
 * 작성자: 윤정도
 * 생성일: 10/20/2023 5:52:31 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>
#include <TF/Common/Structure.h>

class Room
{
public:
	Room();

	JCORE_GETTER_SETTER(L, const RoomInfo&, RoomInfo, m_RoomInfo)
	JCORE_GETTER(L, int, HostCharacterPrimaryKey, m_iHostCharacterPrimaryKey)

	void updateRoomMemberList(RoomCharacterInfo* info, int count, int hostCharacterPrimaryKey);
	void updateRoomInfo(const RoomInfo& info);
	int updateRoomMemberReadyState(int characterPrimaryKey, bool ready);

	RoomCharacterInfo* getRoomMemberByPrimaryKey(int characterPrimaryKey);
	RoomCharacterInfo* getRoomMemberByIndex(int roomMemberIndex);
	int getRoomMemberIndexByPrimaryKey(int characterPrimaryKey);
	int getRoomMemberCount() const { return m_iRoomMemberCount; }
	int getReadyCount() const;

	void leave();

	bool IsBattleEndingState() const;
	bool IsBattlePlayingState() const;
private:
	int m_iRoomMemberCount;
	int m_iHostCharacterPrimaryKey;
	RoomCharacterInfo m_RoomMember[Const::Room::MaxPlayerCount];
	RoomInfo m_RoomInfo;
};
