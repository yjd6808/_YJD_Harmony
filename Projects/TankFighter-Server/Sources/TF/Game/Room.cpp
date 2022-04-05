#define _WINSOCKAPI_

#include <TF/Game/Room.h>
#include <TF/Game/Player.h>

using namespace JCore;

Room::Room(Channel* channel, Player* host, const String& roomName, int maxPlayerCount)
	: m_iRoomUID(ms_iRoomSeq++)
	, m_iMaxPlayerCount(maxPlayerCount)
	, m_pChannel(channel)
	, m_RoomName(roomName)
	, m_pHost(host) {


}

int Room::GetPlayerCount() {
	CriticalSectionLockGuard guard(m_RoomLock);
	return m_PlayerList.Size() + 1;		// 호스트 포함해서
}

bool Room::TryAddPlayer(Player* player) {
	CriticalSectionLockGuard guard(m_RoomLock);
	const int iPlayerCount = m_PlayerList.Size() + 1;

	if (iPlayerCount < m_iMaxPlayerCount) {
		player->Lock();
		player->m_iRoomUID = m_iRoomUID;
		player->m_ePlayerState = PlayerState::RoomReady;
		player->m_bReady = false;
		player->Unlock();
		m_PlayerList.PushBack(player);
		return true;
	}

	return false;
}

bool Room::RemovePlayer(Player* player) {
	CriticalSectionLockGuard guard(m_RoomLock);

	if (player == m_pHost) {
		m_pHost = m_PlayerList.Front();
		m_PlayerList.PopFront();
	}

	return m_PlayerList.Remove(player);
}

bool Room::ChangeHost(Player* player) {
	CriticalSectionLockGuard guard(m_RoomLock);

	if (!m_PlayerList.Exist(player))
		return false;

	player->SetRoomHost(true);
	m_PlayerList.Remove(player);
	m_pHost = player;
	return true;
}

bool Room::ChangeNextHost() {
	CriticalSectionLockGuard guard(m_RoomLock);

	if (m_PlayerList.Size() == 0) {
		return false;
	}

	m_pHost = m_PlayerList.Front();
	m_PlayerList.PopFront();
	return true;
}

Player* Room::GetHost() {
	CriticalSectionLockGuard guard(m_RoomLock);
	return static_cast<Player*>(m_pHost);
}

void Room::BroadcastPacket(JNetwork::ISendPacket* packet, Player* exceptPlayer) {
	packet->AddRef(); // 패킷 베리어
	m_PlayerList.Extension().ForEach([packet, exceptPlayer](Player* p) {
		if (p == exceptPlayer)
			return;

		p->Session()->SendAsync(packet);
	});
	static_cast<Player*>(m_pHost)->Session()->SendAsync(packet);
	packet->Release();
}

void Room::SetRoomState(RoomState state) {
	CriticalSectionLockGuard guard(m_RoomLock);
	m_eRoomState = state;
}

void Room::ForEach(Action<Player*> foreachAction) {
	CriticalSectionLockGuard guard(m_RoomLock);
	m_PlayerList.Extension().ForEach(foreachAction);
}
