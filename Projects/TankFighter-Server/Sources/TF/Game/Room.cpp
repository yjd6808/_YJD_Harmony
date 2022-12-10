/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Game/Room.h>
#include <TF/Game/Player.h>

#include "JCore/Utils/Console.h"

using namespace JCore;


Room::Room()
	: m_iRoomUID(INVALID_UID)
	, m_iMaxPlayerCount(0)
	, m_pChannel(nullptr)
	, m_RoomName(0)					// 빈 문자열로 둠 - 풀에서 꺼낼때 문자열 할당됨
	, m_pHost(nullptr)
{
}

void Room::Initialize(Channel* channel, Player* host, const String& roomName, int maxPlayerCount) {
	RecursiveLockGuard guard(m_RoomLock);
	m_iRoomUID = ms_iRoomSeq++;
	m_iMaxPlayerCount = maxPlayerCount;
	m_pChannel = Move(channel);
	m_RoomName = roomName;
	m_pHost = host;
	m_PlayerList.Clear();
	m_eRoomState = RoomState::ReadyWait;
}

int Room::GetPlayerCount() {
	RecursiveLockGuard guard(m_RoomLock);

	if (m_pHost) {
		return m_PlayerList.Size() + 1;
	}

	return m_PlayerList.Size();
}

// 이미 방안에 있는 경우 또는 꽉 찬 경우 실패
bool Room::TryJoin(Player* player) {
	RecursiveLockGuard guard(m_RoomLock);

	// 게임이 끝나가고 있는 경우에는 참가 못하도록 함
	if (IsBattleFieldEndingState()) {
		return false;
	}

	const int iPlayerCount = m_PlayerList.Size() + (m_pHost ? 1 : 0);

	// 이러면 안댐
	if (m_PlayerList.Exist(player)) {
		DebugAssertMessage(false, "이미 방안에 당신이 들어있습니다.");
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
	RecursiveLockGuard guard(m_RoomLock);

	if (player == m_pHost || m_PlayerList.Exist(player)) {
		player->Lock();
		player->UnsafeSetRoomUID(INVALID_UID);				// 속한 방 정보 없앰
		player->m_bRoomHost = false;						// 방 호스트 정보 없앰
		player->UnsafeSetPlayerState(PlayerState::Lobby);	// 로비 상태로 변경
		player->UnsafeSetReady(false);						// 준비 안됨 상태로 변경
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
	RecursiveLockGuard guard(m_RoomLock);

	if (!m_PlayerList.Exist(player))
		return false;

	player->SetRoomHost(true);
	m_PlayerList.Remove(player);
	m_pHost = player;
	return true;
}

bool Room::ChangeNextHost() {
	RecursiveLockGuard guard(m_RoomLock);

	if (m_PlayerList.Size() == 0) {
		return false;
	}

	m_pHost = m_PlayerList.Front();
	m_PlayerList.PopFront();
	return true;
}

Player* Room::GetHost() {
	RecursiveLockGuard guard(m_RoomLock);
	return static_cast<Player*>(m_pHost);
}

void Room::Broadcast(JNetwork::ISendPacket* packet, Player* exceptPlayer) {
	RecursiveLockGuard guard(m_RoomLock);
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

void Room::BroadcastInBattle(JNetwork::ISendPacket* packet, Player* exceptPlayer) {
	RecursiveLockGuard guard(m_RoomLock);
	packet->AddRef(); // 패킷 베리어
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

bool Room::IsBattleFieldPlayingState() {
	RecursiveLockGuard guard(m_RoomLock);
	return m_eRoomState >= RoomState::PlayWait && m_eRoomState <= RoomState::Playing;
}


bool Room::IsBattleFieldEndingState() {
	RecursiveLockGuard guard(m_RoomLock);
	return m_eRoomState >= RoomState::EndWait;
}

int Room::GetRoomBattleStateUserCount() {
	RecursiveLockGuard guard(m_RoomLock);
	int iCount = 0;
	ForEach([&iCount](Player* p) {
		if (p->GetPlayerState() == PlayerState::RoomBattle && !p->IsDeath()) {
			iCount++;
		}
	});
	return iCount;
}

void Room::ForEach(Action<Player*> foreachAction) {
	m_PlayerList.Extension().ForEach(foreachAction);

	if (m_pHost) {
		foreachAction(GetHost());
	}
}


void Room::SetRoomState(RoomState state) {
	RecursiveLockGuard guard(m_RoomLock);
	m_eRoomState = state;
}

RoomState Room::GetRoomState() {
	RecursiveLockGuard guard(m_RoomLock);
	return m_eRoomState;
}

bool Room::IsBattleFieldState() {
	RecursiveLockGuard guard(m_RoomLock);
	return m_eRoomState >= RoomState::PlayWait;
}




void Room::LoadRoomInfo(Out_ RoomInfo& info) {
	RecursiveLockGuard guard(m_RoomLock);
	info.RoomUID = m_iRoomUID;
	strcpy_s(info.Name, NAME_LEN, m_RoomName.Source());
	info.MaxPlayerCount = m_iMaxPlayerCount;
	info.PlayerCount = m_PlayerList.Size() + 1;
	info.RoomState = m_eRoomState;
}

void Room::SetTimerTime(int time) {
	RecursiveLockGuard guard(m_RoomLock);
	m_iTimerTime = time;
}

int Room::AddTimerTime(int time) {
	RecursiveLockGuard guard(m_RoomLock);
	return m_iTimerTime += time;
}

int Room::GetTimerTime() {
	RecursiveLockGuard guard(m_RoomLock);
	return m_iTimerTime;
}

int Room::SubtractTimerTime(int time) {
	RecursiveLockGuard guard(m_RoomLock);
	return m_iTimerTime -= time;
}


