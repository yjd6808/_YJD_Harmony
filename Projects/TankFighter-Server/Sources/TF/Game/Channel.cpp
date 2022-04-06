#define _WINSOCKAPI_

#include <TF/Game/Channel.h>
#include <TF/Util/Console.h>
#include <TF/Game/Player.h>
#include <JNetwork/Packet.h>

using namespace JCore;
using namespace JNetwork;

Channel::Channel(int channedUID, const String& channelName, int maxPlayerCount)
	: m_ChannelUID(channedUID)
	, m_ChannelName(channelName)
	, m_iMaxPlayerCount(maxPlayerCount)
	, m_PlayerList(30) {
}



int Channel::GetPlayerCount() {
	CriticalSectionLockGuard guard(m_ChannelLock);
	return m_PlayerList.Size();
}


bool Channel::TryAddPlayer(Player* player) {
	CriticalSectionLockGuard guard(m_ChannelLock);
	if (m_PlayerList.Size() < m_iMaxPlayerCount) {
		player->SetChannelUID(m_ChannelUID);
		m_PlayerList.PushBack(player);
		return true;
	}

	return false;
}

bool Channel::RemovePlayer(Player* player) {
	CriticalSectionLockGuard guard(m_ChannelLock);

	const int iRoomUID = player->GetRoomUID();

	if (iRoomUID != INVALID_UID) {
		if (!LeaveRoom(iRoomUID, player)) {
			DebugAssert(false, "�÷��̾ ���� ���� UID�� �̻��մϴ�.");
			return false;
		}
	}

	player->SetChannelUID(INVALID_UID);

	return m_PlayerList.Remove(player);
}

bool Channel::IsPlayerExistByCharacterUID(int characterUID) {
	CriticalSectionLockGuard guard(m_ChannelLock);

	for (int i = 0; i < m_PlayerList.Size(); i++) {
		if (m_PlayerList[i]->GetCharacterUID() == characterUID) {
			return true;
		}
	}

	return false;
}

Player* Channel::FindPlayerByCharacterUID(int characterUID) {
	CriticalSectionLockGuard guard(m_ChannelLock);

	for (int i = 0; i < m_PlayerList.Size(); i++) {
		if (m_PlayerList[i]->GetCharacterUID() == characterUID) {
			return m_PlayerList[i];
		}
	}

	return nullptr;
}

void Channel::PlayerForEach(Action<Player*> foreachAction) {
	CriticalSectionLockGuard guard(m_ChannelLock);
	m_PlayerList.Extension().ForEach(foreachAction);
}

Player* Channel::PlayerFindIf(Func<bool, Player*> predicate) {
	CriticalSectionLockGuard guard(m_ChannelLock);
	const auto ppFind = m_PlayerList.Extension().FindIf(predicate);

	if (ppFind == nullptr) {
		return nullptr;
	}

	return *ppFind;
}

Room* Channel::CreateRoom(Player* creator, const String& roomName, int maxPlayerCount) {
	Room* pNewRoom = new Room(this, creator, roomName, maxPlayerCount);
	CriticalSectionLockGuard guard(m_RoomMapLock);
	if (!m_RoomMap.Insert(pNewRoom->GetRoomUID(), pNewRoom))
		return nullptr;

	creator->Lock();
	creator->m_iRoomUID = pNewRoom->GetRoomUID();
	creator->m_bRoomHost = true;
	creator->m_ePlayerState = PlayerState::RoomReady;
	creator->Unlock();
	return pNewRoom;
}

void Channel::BroadcastLobbyPacket(ISendPacket* packet) {
	CriticalSectionLockGuard guard(m_ChannelLock);

	packet->AddRef(); // ��Ŷ ������
	m_PlayerList.Extension().ForEach([packet](Player* p) {
		if (p->GetPlayerState() == PlayerState::Lobby)
			p->Session()->SendAsync(packet);
	});
	packet->Release();
}

bool Channel::RemoveRoom(const int roomUID) {
	CriticalSectionLockGuard guard(m_RoomMapLock);

	if (!m_RoomMap.Exist(roomUID)) {
		return false;
	}

	Room* pRoom = m_RoomMap[roomUID];
	DeleteSafe(pRoom);

	return m_RoomMap.Remove(roomUID);;
}

Room* Channel::GetRoom(const int roomUID) {
	CriticalSectionLockGuard guard(m_RoomMapLock);

	if (!m_RoomMap.Exist(roomUID)) {
		return nullptr;
	}

	return m_RoomMap[roomUID];
}

// nullptr : ���� �������� �ʰų� �� �� ���
Room* Channel::JoinRoom(const int roomUID, Player* player) {
	CriticalSectionLockGuard guard(m_RoomMapLock);

	if (!m_RoomMap.Exist(roomUID)) {
		return nullptr;
	}

	Room* pRoom = m_RoomMap[roomUID];

	if (pRoom->TryAddPlayer(player)) {
		return pRoom;
	}

	return nullptr;
}

bool Channel::LeaveRoom(const int roomUID, Player* player) {
	CriticalSectionLockGuard guard(m_RoomMapLock);

	if (!m_RoomMap.Exist(roomUID)) {
		return false;
	}

	Room* pRoom = m_RoomMap[roomUID];
	if (!pRoom->RemovePlayer(player)) {
		DebugAssert(false, "��ȿ� �ش� �÷��̾ �����ϴ�. ����");
		return false;
	}

	player->Lock();
	player->m_iRoomUID = INVALID_UID;				// ���� �� ���� ����
	player->m_bRoomHost = false;					// �� ȣ��Ʈ ���� ����
	player->m_ePlayerState = PlayerState::Lobby;	// �κ� ���·� ����
	player->m_bReady = false;						// �غ� �ȵ� ���·� ����
	player->Unlock();
	return true;
}

void Channel::RoomForEach(Action<Room*> foreachAction) {
	CriticalSectionLockGuard guard(m_RoomMapLock);
	m_RoomMap.Values().Extension().ForEach(foreachAction);
}


