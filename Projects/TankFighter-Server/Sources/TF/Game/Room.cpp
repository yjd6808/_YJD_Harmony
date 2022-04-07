#define _WINSOCKAPI_

#include <TF/Game/Room.h>
#include <TF/Game/Player.h>

using namespace JCore;


Room::Room()
	: m_iRoomUID(INVALID_UID)
	, m_iMaxPlayerCount(0)
	, m_pChannel(nullptr)
	, m_RoomName(0)					// 빈 문자열로 둠 - 풀에서 꺼낼때 문자열 할당됨
	, m_pHost(nullptr)
{
}

void Room::Initialize(Channel* channel, Player* host, const JCore::String& roomName, int maxPlayerCount) {
	CriticalSectionLockGuard guard(m_RoomLock);
	m_iRoomUID = ms_iRoomSeq++;
	m_iMaxPlayerCount = maxPlayerCount;
	m_pChannel = Move(channel);
	m_RoomName = roomName;
	m_pHost = host;
	m_PlayerList.Clear();
	m_eRoomState = RoomState::ReadyWait;
}

int Room::GetPlayerCount() {
	CriticalSectionLockGuard guard(m_RoomLock);

	if (m_pHost) {
		return m_PlayerList.Size() + 1;
	}

	return m_PlayerList.Size();
}

// 이미 방안에 있는 경우 또는 꽉 찬 경우 실패
bool Room::TryAddPlayer(Player* player) {
	CriticalSectionLockGuard guard(m_RoomLock);
	const int iPlayerCount = m_PlayerList.Size() + (m_pHost ? 1 : 0);

	// 이러면 안댐
	if (m_PlayerList.Exist(player)) {
		DebugAssert(false, "이미 방안에 당신이 들어있습니다.");
		return false;
	}

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

	if (player == m_pHost || m_PlayerList.Exist(player)) {
		player->Lock();
		player->m_iRoomUID = INVALID_UID;				// 속한 방 정보 없앰
		player->m_bRoomHost = false;					// 방 호스트 정보 없앰
		player->m_ePlayerState = PlayerState::Lobby;	// 로비 상태로 변경
		player->m_bReady = false;						// 준비 안됨 상태로 변경
		player->Unlock();

		if (m_pHost == player) {
			m_pHost = nullptr;

			if (!m_PlayerList.IsEmpty()) {
				// 먼저 접속한 유저 순서대로 방장 교체해줌
				m_pHost = m_PlayerList.Front();
				m_PlayerList.PopFront();
			}
			
		} else {
			m_PlayerList.Remove(player);
		}
		return true;
	}

	return false;
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

	if (m_pHost)
		GetHost()->Session()->SendAsync(packet);

	packet->Release();
}

void Room::SetRoomState(RoomState state) {
	CriticalSectionLockGuard guard(m_RoomLock);
	m_eRoomState = state;
}

void Room::ForEach(Action<Player*> foreachAction) {
	CriticalSectionLockGuard guard(m_RoomLock);
	m_PlayerList.Extension().ForEach(foreachAction);

	if (m_pHost) {
		foreachAction(GetHost());
	}
}


void Room::LoadRoomInfo(Out_ RoomInfo& info) {
	CriticalSectionLockGuard guard(m_RoomLock);
	info.RoomUID = m_iRoomUID;
	strcpy_s(info.Name, NAME_LEN, m_RoomName.Source());
	info.MaxPlayerCount = m_iMaxPlayerCount;
	info.PlayerCount = m_PlayerList.Size() + 1;
}
