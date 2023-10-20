/*
 * 작성자: 윤정도
 * 생성일: 10/20/2023 5:52:31 PM
 * =====================
 *
 */

#include "Pch.h"
#include "Room.h"

USING_NS_JC;
USING_NS_JNET;

Room::Room()
	: m_iRoomMemberCount(0)
	, m_iHostCharacterPrimaryKey(Const::InvalidValue)
	, m_RoomMember{}
	, m_RoomInfo()
{}

void Room::updateRoomMemberList(RoomCharacterInfo* info, int count, int hostCharacterPrimaryKey) {
	m_iRoomMemberCount = count;
	m_iHostCharacterPrimaryKey = hostCharacterPrimaryKey;
	for (int i = 0; i < count; i++) {
		m_RoomMember[i] = info[i];
	}
}

void Room::updateRoomInfo(const RoomInfo& info) {
	m_RoomInfo = info;
}


int Room::updateRoomMemberReadyState(int characterPrimaryKey, bool ready) {
	const int iMemberIndex = getRoomMemberIndexByPrimaryKey(characterPrimaryKey);
	if (iMemberIndex == Const::InvalidValue) {
		return iMemberIndex;
	}

	m_RoomMember[iMemberIndex].Ready = ready;
	return iMemberIndex;
}

RoomCharacterInfo* Room::getRoomMemberByPrimaryKey(int characterPrimaryKey) {
	int iIndex = Arrays::FindIf(m_RoomMember, [=](RoomCharacterInfo& info) { return info.PrimaryKey == characterPrimaryKey; });
	if (iIndex == Const::InvalidValue) return nullptr;
	return &m_RoomMember[iIndex];
}

RoomCharacterInfo* Room::getRoomMemberByIndex(int roomMemberIndex) {
	if (roomMemberIndex < 0 || roomMemberIndex >= Const::Room::MaxPlayerCount)
		return nullptr;

	return &m_RoomMember[roomMemberIndex];
}

int Room::getRoomMemberIndexByPrimaryKey(int characterPrimaryKey) {
	return Arrays::FindIf(m_RoomMember, [=](RoomCharacterInfo& info) { return info.PrimaryKey == characterPrimaryKey; });
}

int Room::getReadyCount() const {
	int iCount = 0;
	for (int i = 0; i < m_iRoomMemberCount; ++i)
		if (m_RoomMember[i].Ready)
			iCount++;

	return iCount;
}

void Room::leave() {
	m_iRoomMemberCount = 0;
	m_iHostCharacterPrimaryKey = Const::InvalidValue;
}

bool Room::IsBattleEndingState() const {
	return m_RoomInfo.IsBattleEndingState();
}

bool Room::IsBattlePlayingState() const {
	return m_RoomInfo.IsBattlePlayingState();
}
