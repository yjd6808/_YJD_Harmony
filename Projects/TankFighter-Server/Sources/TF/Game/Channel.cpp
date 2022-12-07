/*
 * 작성자 : 윤정도
 */




#include <TF/PrecompiledHeader.h>
#include <TF/Game/Channel.h>
#include <TF/Game/Player.h>
#include <TF/ServerConfiguration.h>

#include <JCore/Utils/Console.h>
#include <JNetwork/Packet.h>


using namespace JCore;
using namespace JNetwork;

Channel::Channel(int channedUID, const String& channelName, int maxPlayerCount)
	: m_ChannelUID(channedUID)
	, m_ChannelName(channelName)
	, m_iMaxPlayerCount(maxPlayerCount)
	, m_PlayerList(PLAYER_POOL_SIZE)
	, m_RoomPool(this)
	, m_BattleFieldWorker(m_ChannelUID, BATTLE_FIELD_WORKER_DELAY)
{
}

Channel::~Channel() {
}

bool Channel::Initialize() {
	if (!m_RoomPool.Initialize(ROOM_POOL_SIZE)) {
		return false;
	}

	m_BattleFieldWorker.Run();
	return true;
}

bool Channel::Finalize() {
	m_BattleFieldWorker.Join();		// 방삭제전 먼저 조인 해줘야함

	return m_RoomPool.Finalize();
}


int Channel::GetPlayerCount() {
	NormalLockGuard guard(m_ChannelLock);
	return m_PlayerList.Size();
}


bool Channel::TryAddPlayer(Player* player) {
	NormalLockGuard guard(m_ChannelLock);
	if (m_PlayerList.Size() < m_iMaxPlayerCount) {
		player->SetChannelUID(m_ChannelUID);
		m_PlayerList.PushBack(player);
		return true;
	}

	return false;
}

bool Channel::RemovePlayer(Player* player) {
	NormalLockGuard guard(m_ChannelLock);

	const int iRoomUID = player->GetRoomUID();

	if (iRoomUID != INVALID_UID) {
		if (!LeaveRoom(player)) {
			DebugAssertMessage(false, "플레이어가 속한 방의 UID가 이상합니다.");
			return false;
		}
	}

	player->SetChannelUID(INVALID_UID);

	return m_PlayerList.Remove(player);
}

bool Channel::IsPlayerExistByCharacterUID(int characterUID) {
	NormalLockGuard guard(m_ChannelLock);

	for (int i = 0; i < m_PlayerList.Size(); i++) {
		if (m_PlayerList[i]->GetCharacterUID() == characterUID) {
			return true;
		}
	}

	return false;
}

Player* Channel::FindPlayerByCharacterUID(int characterUID) {
	NormalLockGuard guard(m_ChannelLock);

	for (int i = 0; i < m_PlayerList.Size(); i++) {
		if (m_PlayerList[i]->GetCharacterUID() == characterUID) {
			return m_PlayerList[i];
		}
	}

	return nullptr;
}

void Channel::PlayerForEach(Action<Player*> foreachAction) {
	NormalLockGuard guard(m_ChannelLock);
	m_PlayerList.Extension().ForEach(foreachAction);
}

Player* Channel::PlayerFindIf(Func<bool, Player*> predicate) {
	NormalLockGuard guard(m_ChannelLock);
	const auto ppFind = m_PlayerList.Extension().FindIf(predicate);

	if (ppFind == nullptr) {
		return nullptr;
	}

	return *ppFind;
}

Room* Channel::CreateRoom(Player* creator, const String& roomName, int maxPlayerCount) {
	NormalLockGuard guard(m_RoomMapLock);
	Room* pNewRoom = m_RoomPool.PopRoom(creator, roomName, maxPlayerCount);
	if (!m_RoomMap.Insert(pNewRoom->GetRoomUID(), pNewRoom))
		return nullptr;

	creator->Lock();
	creator->UnsafeSetRoomUID(pNewRoom->GetRoomUID());
	creator->m_bRoomHost = true;
	creator->UnsafeSetPlayerState(PlayerState::RoomLobby);
	creator->Unlock();
	return pNewRoom;
}

void Channel::BroadcastLobbyPacket(ISendPacket* packet) {
	NormalLockGuard guard(m_ChannelLock);

	packet->AddRef(); // 패킷 베리어
	m_PlayerList.Extension().ForEach([packet](Player* p) {
		if (p->GetPlayerState() == PlayerState::Lobby)
			p->Session()->SendAsync(packet);
	});
	packet->Release();
}

bool Channel::RemoveRoom(const int roomUID) {
	NormalLockGuard guard(m_RoomMapLock);

	if (!m_RoomMap.Exist(roomUID)) {
		return false;
	}

	Room* pRoom = m_RoomMap[roomUID];

	if (pRoom->IsBattleFieldState()) {
		m_BattleFieldWorker.RemoveBattleFieldRoom(pRoom);
	}

	m_RoomPool.ReleaseRoom(pRoom);
	return m_RoomMap.Remove(roomUID);
}

bool Channel::RemoveRoomIfEmpty(Room* room) {
	NormalLockGuard guard(m_RoomMapLock);

	if (room->IsEmpty()) {
		return RemoveRoom(room->GetRoomUID());
	}

	return false;
}

Room* Channel::GetRoomByRoomUID(const int roomUID) {
	NormalLockGuard guard(m_RoomMapLock);

	if (!m_RoomMap.Exist(roomUID)) {
		return nullptr;
	}

	return m_RoomMap[roomUID];
}

Room* Channel::GetRoomByPlayer(Player* player) {
	NormalLockGuard guard(m_RoomMapLock);

	int iRoomUID = player->GetRoomUID();
	if (!m_RoomMap.Exist(iRoomUID)) {
		return nullptr;
	}

	return m_RoomMap[iRoomUID];
}

// nullptr : 방이 존재하지 않거나 꽉 찬 경우
Room* Channel::JoinRoom(const int roomUID, Player* player) {
	NormalLockGuard guard(m_RoomMapLock);

	if (!m_RoomMap.Exist(roomUID)) {
		return nullptr;
	}

	Room* pRoom = m_RoomMap[roomUID];

	if (pRoom->TryJoin(player)) {
		return pRoom;
	}

	return nullptr;
}

bool Channel::LeaveRoom(Player* player) {
	NormalLockGuard guard(m_RoomMapLock);

	const int iRoomUID = player->GetRoomUID();

	if (!m_RoomMap.Exist(iRoomUID)) {
		return false;
	}

	Room* pRoom = m_RoomMap[iRoomUID];
	if (!pRoom->RemovePlayer(player)) {
		DebugAssertMessage(false, "방안에 해당 플레이어가 없습니다. ㄷㄷ");
		return false;
	}

	if (pRoom->IsEmpty()) {
		if (pRoom->IsBattleFieldState()) {
			if (!m_BattleFieldWorker.RemoveBattleFieldRoom(pRoom)) {
				DebugAssertMessage(false, "배틀 필드 상태인데 배틀필드 워커에 없는 방입니다.");
			}
		}

		m_RoomPool.ReleaseRoom(pRoom);
		return m_RoomMap.Remove(iRoomUID);
	}
	
	return true;
}

void Channel::RoomForEach(Action<Room*> foreachAction) {
	NormalLockGuard guard(m_RoomMapLock);
	m_RoomMap.Values().Extension().ForEach(foreachAction);
}


int Channel::GetRoomCount() {
	NormalLockGuard guard(m_RoomMapLock);
	return m_RoomMap.Size();
}

int Channel::GetPlayingRoomCount() {
	int iCnt = 0;
	RoomForEach([&iCnt](Room* r) {
		if (r->IsBattleFieldState())
			iCnt++;
	});
	return iCnt;
}

bool Channel::StartBattle(Room* pRoom) {

	if (pRoom->IsBattleFieldState()) {
		return false;
	}

	pRoom->Lock();

	pRoom->m_eRoomState = RoomState::PlayWait;
	pRoom->m_iTimerTime = BATTLE_FIELD_PLAYWAIT_TIME;
	pRoom->UnsafeForEach([](Player* p) {
		p->InitializeRoomBattleState();
	});
	pRoom->Unlock();
	m_BattleFieldWorker.AddBattleFieldRoom(pRoom);
	return true;
}


