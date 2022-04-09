/*
 * �ۼ��� : ������
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Game/Room.h>
#include <TF/Game/Player.h>

using namespace JCore;


Room::Room()
	: m_iRoomUID(INVALID_UID)
	, m_iMaxPlayerCount(0)
	, m_pChannel(nullptr)
	, m_RoomName(0)					// �� ���ڿ��� �� - Ǯ���� ������ ���ڿ� �Ҵ��
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

// �̹� ��ȿ� �ִ� ��� �Ǵ� �� �� ��� ����
bool Room::TryJoin(Player* player) {
	CriticalSectionLockGuard guard(m_RoomLock);

	// ������ �������� �ִ� ��쿡�� ���� ���ϵ��� ��
	if (UnsafeIsBattleFieldEndingState()) {
		return false;
	}

	const int iPlayerCount = m_PlayerList.Size() + (m_pHost ? 1 : 0);

	// �̷��� �ȴ�
	if (m_PlayerList.Exist(player)) {
		DebugAssert(false, "�̹� ��ȿ� ����� ����ֽ��ϴ�.");
		return false;
	}

	if (iPlayerCount < m_iMaxPlayerCount) {
		player->InitializeRoomLobbyState(m_iRoomUID);
		m_PlayerList.PushBack(player);
		return true;
	}

	return false;
}

bool Room::RemovePlayer(Player* player) {
	CriticalSectionLockGuard guard(m_RoomLock);

	if (player == m_pHost || m_PlayerList.Exist(player)) {
		player->Lock();
		player->m_iRoomUID = INVALID_UID;				// ���� �� ���� ����
		player->m_bRoomHost = false;					// �� ȣ��Ʈ ���� ����
		player->m_ePlayerState = PlayerState::Lobby;	// �κ� ���·� ����
		player->m_bReady = false;						// �غ� �ȵ� ���·� ����
		player->Unlock();

		if (m_pHost == player) {
			m_pHost = nullptr;

			if (!m_PlayerList.IsEmpty()) {
				// ���� ������ ���� ������� ���� ��ü����
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

void Room::Broadcast(JNetwork::ISendPacket* packet, Player* exceptPlayer) {
	CriticalSectionLockGuard guard(m_RoomLock);
	packet->AddRef(); // ��Ŷ ������
	m_PlayerList.Extension().ForEach([packet, exceptPlayer](Player* p) {
		if (p == exceptPlayer)
			return;

		p->Session()->SendAsync(packet);
	});

	if (m_pHost)
		GetHost()->Session()->SendAsync(packet);

	packet->Release();
}

void Room::UnsafeBroadcast(JNetwork::ISendPacket* packet, Player* exceptPlayer) {
	packet->AddRef(); // ��Ŷ ������
	m_PlayerList.Extension().ForEach([packet, exceptPlayer](Player* p) {
		if (p == exceptPlayer)
			return;

		p->Session()->SendAsync(packet);
		});

	if (m_pHost)
		GetHost()->Session()->SendAsync(packet);
	packet->Release();
}

void Room::UnsafeBroadcastInBattle(JNetwork::ISendPacket* packet, Player* exceptPlayer) {
	packet->AddRef(); // ��Ŷ ������
	m_PlayerList.Extension().ForEach([packet, exceptPlayer](Player* p) {
		if (p == exceptPlayer)
			return;

		if (p->GetPlayerState() == PlayerState::RoomBattle && p->IsReady())
			p->Session()->SendAsync(packet);
		});

	if (m_pHost)
		GetHost()->Session()->SendAsync(packet);
	packet->Release();
}

bool Room::UnsafeIsBattleFieldState() const {
	return m_eRoomState >= RoomState::PlayWait;
}

bool Room::UnsafeIsBattleFieldPlayingState() const {
	return m_eRoomState >= RoomState::PlayWait && m_eRoomState <= RoomState::Playing;
}


bool Room::UnsafeIsBattleFieldEndingState() const {
	return m_eRoomState >= RoomState::EndWait;
}

void Room::UnsafeForEach(JCore::Action<Player*> foreachAction) {
	m_PlayerList.Extension().ForEach(foreachAction);

	if (m_pHost) {
		foreachAction(GetHost());
	}
}


void Room::SetRoomState(RoomState state) {
	CriticalSectionLockGuard guard(m_RoomLock);
	m_eRoomState = state;
}

void Room::ForEach(Action<Player*> foreachAction) {
	m_PlayerList.Extension().ForEach(foreachAction);

	if (m_pHost) {
		foreachAction(GetHost());
	}
}


RoomState Room::GetRoomState() {
	CriticalSectionLockGuard guard(m_RoomLock);
	return m_eRoomState;
}

bool Room::IsBattleFieldState() {
	CriticalSectionLockGuard guard(m_RoomLock);
	return m_eRoomState >= RoomState::PlayWait;
}




void Room::LoadRoomInfo(Out_ RoomInfo& info) {
	CriticalSectionLockGuard guard(m_RoomLock);
	info.RoomUID = m_iRoomUID;
	strcpy_s(info.Name, NAME_LEN, m_RoomName.Source());
	info.MaxPlayerCount = m_iMaxPlayerCount;
	info.PlayerCount = m_PlayerList.Size() + 1;
	info.RoomState = m_eRoomState;
}

void Room::SetTimerTime(int time) {
	CriticalSectionLockGuard guard(m_RoomLock);
	m_iTimerTime = time;
}

int Room::AddTimerTime(int time) {
	CriticalSectionLockGuard guard(m_RoomLock);
	return m_iTimerTime += time;
}

int Room::GetTimerTime() {
	CriticalSectionLockGuard guard(m_RoomLock);
	return m_iTimerTime;
}

int Room::SubtractTimerTime(int time) {
	CriticalSectionLockGuard guard(m_RoomLock);
	return m_iTimerTime -= time;
}


