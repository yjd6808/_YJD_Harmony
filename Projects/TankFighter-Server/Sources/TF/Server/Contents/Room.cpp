/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:23:01 PM
 * =====================
 *
 */


#include "Pch.h"
#include "Room.h"

#include <TF/Server/Const.h>

USING_NS_JC;

Room::Room()
	: m_vPlayerList(0)			// Lazy-Intialization
	, m_pLeader(nullptr)
{}

void Room::OnUpdate(const TimeSpan& elapsed) {
}

void Room::BroadcastPacket(JNetwork::ISendPacket* packet, int state) {
}

void Room::OnPopped() {
	m_bClosed = false;
	m_eState = RoomState::ReadyWait;
}

void Room::OnPushed() {
	m_bClosed = true;
	m_pLeader = nullptr;
	m_vPlayerList.Clear();
}

void Room::Close() {
}

void Room::Open() {
}

void Room::SetName(const String& name) {
	JCORE_LOCK_GUARD(m_Sync);
	m_szName = name;
}

void Room::SetNameRaw(const char* name) {
	m_szName = name;
}

void Room::SetNameRaw(const String& name) {
	m_szName = name;
}

String Room::GetName() {
	String szName = String::Null;
	{
		JCORE_LOCK_GUARD(m_Sync);
		szName = m_szName;
	}
	return szName;
}

int Room::GetPlayerCount() {
	JCORE_LOCK_GUARD(m_Sync);
	return m_vPlayerList.Size();
}

bool Room::IsEmpty() {
	JCORE_LOCK_GUARD(m_Sync);
	return m_vPlayerList.Size() <= 0;
}

int Room::Join(Player* player) {
	{
		JCORE_LOCK_GUARD(m_Sync);

		if (m_bClosed) {
			return Const::Failed::Room::AddFailedClosed;
		}

		const int iResult = AddPlayerRaw(player);
		if (iResult != Const::Success)
			return iResult;

		if (m_vPlayerList.Size() == 1) {
			m_pLeader = player;
		}
	}

	player->OnRoomJoin(this);
	return Const::Success;
}

bool Room::Leave(Player* player) {
	NormalLockGuard guard(m_Sync);

	const bool bPlayerRemoved = RemovePlayerRaw(player);
	const bool bHost = m_pLeader == player;
	const bool bEmpty = IsEmptyRaw();

	if (bPlayerRemoved) {
		m_Sync.Unlock();
		player->OnRoomLeave();
		m_Sync.Lock();
	}

	if (bEmpty) {
		Push(this);
		return bPlayerRemoved;
	}

	if (bHost && bPlayerRemoved)
		ChooseNewLeaderRaw();

	return bPlayerRemoved;
}

bool Room::AddPlayer(Player* player) {
	JCORE_LOCK_GUARD(m_Sync);
	return AddPlayerRaw(player);
}

bool Room::RemovePlayer(Player* player) {
	JCORE_LOCK_GUARD(m_Sync);
	return RemovePlayerRaw(player);
}

RoomInfo Room::GetRoomInfo() {
	RoomInfo info;
	info.AccessId = m_iAccessId;
	info.MaxPlayerCount = Const::Room::MaxPlayerCount;
	info.RoomState = m_eState;
	{
		JCORE_LOCK_GUARD(m_Sync);
		info.PlayerCount = m_vPlayerList.Size();
		info.Name = m_szName;
	}
	return info;
}

void Room::ChooseNewLeaderRaw() {
	const int iLeftPlayerCount = m_vPlayerList.Size();
	if (iLeftPlayerCount > 0) {
		m_pLeader = m_vPlayerList[0];
	}
}

int Room::AddPlayerRaw(Player* player) {

	if (m_vPlayerList.Size() >= Const::Room::MaxPlayerCount) {
		return Const::Failed::Room::AddFailedFull;
	}

	for (int i = 0; i < m_vPlayerList.Size(); ++i) {
		if (m_vPlayerList[i] == player)
			return Const::Failed::Room::AddFailedAlreadyExist;
	}

	m_vPlayerList.PushBack(player);
	return Const::Success;
}

bool Room::RemovePlayerRaw(Player* player) {
	return m_vPlayerList.Remove(player);
}

int Room::GetPlayerCountRaw() const {
	return m_vPlayerList.Size();
}

bool Room::IsEmptyRaw() {
	return m_vPlayerList.Size() == 0;
}
