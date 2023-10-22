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

void Room::setState(RoomState state) {
	m_RoomInfo.RoomState = state;
}

void Room::updateRoomMemberList(RoomCharacterInfo* info, int count, int hostCharacterPrimaryKey) {
	m_iRoomMemberCount = count;
	m_iHostCharacterPrimaryKey = hostCharacterPrimaryKey;
	for (int i = 0; i < count; i++) {
		m_RoomMember[i] = info[i];
	}
}

void Room::updateRoomMember(const RoomCharacterInfo& info) {
	RoomCharacterInfo* pInfo = getRoomMemberByPrimaryKey(info.PrimaryKey);
	if (pInfo == nullptr) {
		_LogWarn_("%d 룸캐릭터 정보 업데이트 실패\n");
		return;
	}
	*pInfo = info;
}

void Room::updateRoomInfo(const RoomInfo& info) {
	m_RoomInfo = info;
}


int Room::updateRoomMemberReadyState(int characterPrimaryKey, bool ready) {
	const int iMemberIndex = getRoomMemberIndexByPrimaryKey(characterPrimaryKey);
	if (iMemberIndex == Const::InvalidValue) {
		return iMemberIndex;
	}

	m_RoomMember[iMemberIndex].IsReady = ready;
	return iMemberIndex;
}

RoomCharacterInfo* Room::getRoomMemberByPrimaryKey(int characterPrimaryKey) {
	int iIndex = Arrays::FindIf(m_RoomMember, [=](const RoomCharacterInfo& info) { return info.PrimaryKey == characterPrimaryKey; });
	if (iIndex == Const::InvalidValue) return nullptr;
	return &m_RoomMember[iIndex];
}

RoomCharacterInfo* Room::getRoomMemberByIndex(int roomMemberIndex) {
	if (roomMemberIndex < 0 || roomMemberIndex >= Const::Room::MaxPlayerCount)
		return nullptr;

	return &m_RoomMember[roomMemberIndex];
}

int Room::getRoomMemberIndexByPrimaryKey(int characterPrimaryKey) {
	return Arrays::FindIf(m_RoomMember, [=](const RoomCharacterInfo& info) { return info.PrimaryKey == characterPrimaryKey; });
}

int Room::getReadyCount() const {
	int iCount = 0;
	for (int i = 0; i < m_iRoomMemberCount; ++i)
		if (m_RoomMember[i].IsReady)
			iCount++;

	return iCount;
}

String Room::getRoomMemberName(int characterPrimaryKey) {
	RoomCharacterInfo* pInfo = getRoomMemberByPrimaryKey(characterPrimaryKey);
	if (pInfo == nullptr) return "(null)";
	return pInfo->Name.Source;
}

bool Room::isBattleEndingState() const {
	return m_RoomInfo.IsBattleEndingState();
}

bool Room::isBattlePlayingState() const {
	return m_RoomInfo.IsBattlePlayingState();
}

bool Room::isDeath(int characterPrimaryKey) {
	const RoomCharacterInfo* pInfo = getRoomMemberByPrimaryKey(characterPrimaryKey);

	// 없으면 죽은걸로 처리
	if (pInfo == nullptr) {
		return true;
	}

	return pInfo->IsDeath;
}
