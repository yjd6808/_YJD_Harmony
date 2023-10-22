/*
 * 작성자: 윤정도
 * 생성일: 10/17/2023 9:07:11 AM
 * =====================
 *
 */

#include "Pch.h"
#include "ChannelLobby.h"

#include <TF/Server/Const.h>
#include <TF/Server/Contents/Character.h>
#include <TF/Server/Send/S_GAME.h>

#include "TF/Common/Command.h"

USING_NS_JC;
USING_NS_JNET;

ChannelLobby::ChannelLobby() {}
ChannelLobby::~ChannelLobby() {}

void ChannelLobby::BroadcastPacket(ISendPacket* packet, int state) {
	auto fnBroadcastAny = [=](Player* player) {
		player->SendPacket(packet);
	};

	auto fnBroadcastLobby = [=](Player* player) {
		if (player->GetPlayerState() == PlayerState::Lobby) 
			player->SendPacket(packet);
	};

	{
		JCORE_REF_COUNT_GUARD(packet);
		JCORE_LOCK_GUARD(m_PlayerListLock);
		if (state == Const::Broadcast::Lobby::StateAny)
			m_hsPlayerSet.ForEach(fnBroadcastAny);
		else if (state == Const::Broadcast::Lobby::StateLobby)
			m_hsPlayerSet.ForEach(fnBroadcastLobby);
	}
}

void ChannelLobby::OnUpdate(const TimeSpan& elapsed) {
	static Vector<Room*> s_vUpdatesRooms;
	{
		JCORE_LOCK_GUARD(m_RoomListLock);
		s_vUpdatesRooms.PushBackAll(m_hsRoomSet);
	}

	const int iSize = s_vUpdatesRooms.Size();
	for (int i = 0; i < iSize; ++i) {
		Room* pRoom = s_vUpdatesRooms[i];
		pRoom->OnUpdate(elapsed);
	}

	s_vUpdatesRooms.Clear();
}

bool ChannelLobby::Join(Player* player) {
	DebugAssert(player);
	const bool bAdded = AddPlayer(player);
	if (bAdded) {
		player->OnLobbyJoin(this);
		S_GAME::SEND_SC_UpdatePlayerListBroadcastInLobby(this);
	}
	return bAdded;
}

bool ChannelLobby::Leave(Player* player) {
	const bool bRemoved = RemovePlayer(player);

	if (bRemoved) {
		player->OnLobbyLeave();
		S_GAME::SEND_SC_UpdatePlayerListBroadcastInLobby(this);
	}

	return bRemoved;
}

bool ChannelLobby::AddPlayer(Player* player) {
	const Character* pCharacter = player->GetCharacter();

	if (pCharacter == nullptr) {
		DebugAssert(false);
		return false;
	}

	JCORE_LOCK_GUARD(m_PlayerListLock);

	if (!m_hsPlayerSet.Insert(player)) 
		return false;

	const String& szName = pCharacter->GetName();
	if (!m_hmPlayerMapByName.Insert(szName, player)) {
		DebugAssert(false, "동일한 이름의 플레이어(%s)가 채널로비에 포함되어 있습니다.", szName.Source());
		m_hsPlayerSet.Remove(player);
		return false;
	}

	if (!m_hmPlayerMapByCharacterPrimaryKey.Insert(pCharacter->GetPrimaryKey(), player)) {
		m_hsPlayerSet.Remove(player);
		m_hmPlayerMapByName.Remove(szName);
		DebugAssert(false, "동일한 캐릭터PK의 플레이어(%s)가 채널로비에 포함되어 있습니다.", szName.Source());
		return false;
	}

	return true;
}

bool ChannelLobby::RemovePlayer(Player* player) {
	if (player == nullptr)
		return false;

	const Character* pCharacter = player->GetCharacter();
	if (pCharacter == nullptr) {
		DebugAssert(false);
		return false;
	}

	JCORE_LOCK_GUARD(m_PlayerListLock);

	const String& szName = pCharacter->GetName();

	m_hsPlayerSet.Remove(player);
	m_hmPlayerMapByName.Remove(szName);
	m_hmPlayerMapByCharacterPrimaryKey.Remove(pCharacter->GetPrimaryKey());
	return true;
}

Player* ChannelLobby::GetPlayerByName(const char* name) {
	JCORE_LOCK_GUARD(m_PlayerListLock);
	Player** ppPlayer = m_hmPlayerMapByName.Find(name);
	if (ppPlayer == nullptr) return nullptr;
	return *ppPlayer;
}

void ChannelLobby::GetPlayerListByCharacterPrimaryKeyList(const Vector<int>& characterPrimaryKeyList, JCORE_REF_OUT Vector<Player*>& playerList) {
	JCORE_LOCK_GUARD(m_PlayerListLock);
	for (int i = 0; i < characterPrimaryKeyList.Size(); ++i) {
		Player** ppPlayer = m_hmPlayerMapByCharacterPrimaryKey.Find(characterPrimaryKeyList[i]);
		if (ppPlayer == nullptr) continue;
		playerList.PushBack(*ppPlayer);
	}
}

Player* ChannelLobby::GetPlayerByCharacterPrimaryKey(int characterPrimaryKey) {
	JCORE_LOCK_GUARD(m_PlayerListLock);
	Player** ppPlayer = m_hmPlayerMapByCharacterPrimaryKey.Find(characterPrimaryKey);
	if (ppPlayer == nullptr) return nullptr;
	return *ppPlayer;
}

bool ChannelLobby::AddRoom(Room* room) {
	if (room == nullptr)
		return false;

	JCORE_LOCK_GUARD(m_RoomListLock);
	return m_hsRoomSet.Insert(room);
}

bool ChannelLobby::RemoveRoom(Room* room) {
	if (room == nullptr)
		return false;

	JCORE_LOCK_GUARD(m_RoomListLock);
	return m_hsRoomSet.Remove(room);
}

Vector<RoomInfo> ChannelLobby::GetRoomInfoList() {
	JCORE_LOCK_GUARD(m_RoomListLock);
	Vector<RoomInfo> vInfoList(m_hsRoomSet.Size());
	m_hsRoomSet.ForEach([&vInfoList](Room* room) {
		RoomInfo info;
		room->GetRoomInfo(info);
		vInfoList.PushBack(Move(info));
	});
	return vInfoList;
}

Vector<CharacterInfo> ChannelLobby::GetPlayerInfoList(PlayerState state) {
	JCORE_LOCK_GUARD(m_RoomListLock);
	Vector<CharacterInfo> vInfoList(m_hsPlayerSet.Size());
	m_hsPlayerSet.ForEach([&vInfoList, state](Player* player) {
		if (player->GetPlayerState() != state)
			return;

		Character* pCharacter = player->GetCharacter();
		if (pCharacter == nullptr)
			return;

		CharacterInfo info;
		pCharacter->GetInfo(info);
		vInfoList.PushBack(info);
	});
	return vInfoList;
}

void ChannelLobby::Finalize() {
	if (m_hsRoomSet.Size() <= 0)
		return;

	_LogWarn_("Finalize중 아직 생성되어있는 방이 존재합니다.");
	m_hsRoomSet.ForEach([](Room* room) {
		Room::Push(room);
	});	
}

void ChannelLobby::BroadcastRoomListInfo() {
	const auto vRoomInfoList = GetRoomInfoList();
	const auto pPacket = dbg_new SinglePacket<SC_UpdateRoomList>(vRoomInfoList.Size());
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	for (int i = 0; i < vRoomInfoList.Size(); ++i) {
		RoomInfo& dst = pPacket->Cmd.Info[i];
		const RoomInfo& src = vRoomInfoList[i];

		dst.AccessId = src.AccessId;
		dst.PlayerCount = src.PlayerCount;
		dst.MaxPlayerCount = src.MaxPlayerCount;
		dst.Name = src.Name;
		dst.RoomState = src.RoomState;
	}

	BroadcastPacket(pPacket, Const::Broadcast::Lobby::StateLobby);
}
