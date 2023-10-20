/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:23:01 PM
 * =====================
 *
 */


#include "Pch.h"
#include "Room.h"

#include <TF/Common/Command.h>
#include <TF/Server/Const.h>

#include "Character.h"
#include "Send/S_GAME.h"

USING_NS_JC;
USING_NS_JNET;

Room::Room()
	: m_eState(RoomState::ReadyWait)			
	, m_szName(0)
	, m_vPlayerList(0)						// Lazy-Intialization
	, m_pLeader(nullptr)
{}

void Room::OnUpdate(const TimeSpan& elapsed) {
}

void Room::BroadcastPacket(ISendPacket* packet, int state) {
	auto fnStateAny = [=](Player* player) {
		player->SendPacket(packet);
	};

	if (state == Const::Broadcast::Room::StateAny) {
		m_vPlayerList.ForEach(fnStateAny);
	}
}

void Room::OnPopped() {
	m_bClosed = false;
	m_eState = RoomState::ReadyWait;
}

void Room::OnPushed() {
	m_pLobby = nullptr;
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

void Room::SetLobby(ChannelLobby* lobby) {
	m_pLobby = lobby;
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
	BroadcastRoomListInfo();
	return Const::Success;
}

bool Room::Leave(Player* player) {
	JCORE_LOCK_GUARD(m_Sync);

	const bool bPlayerRemoved = RemovePlayerRaw(player);
	const bool bHost = m_pLeader == player;
	const bool bEmpty = IsEmptyRaw();



	if (bEmpty) {
		Push(this);
		m_pLobby->RemoveRoom(this);
	} else {
		if (bHost && bPlayerRemoved)
			ChooseNewLeaderRaw();
	}

	if (bPlayerRemoved) {
		m_Sync.Unlock();
		player->OnRoomLeave();

		BroadcastRoomListInfo();
		BroadcastRoomMemberListInfo();

		m_Sync.Lock();
	}

	
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

void Room::GetRoomInfo(JCORE_REF_OUT RoomInfo& info) {
	info.AccessId = m_iAccessId;
	info.MaxPlayerCount = Const::Room::MaxPlayerCount;
	info.RoomState = m_eState;

	{
		JCORE_LOCK_GUARD(m_Sync);
		info.PlayerCount = m_vPlayerList.Size();
		info.Name = m_szName;
	}
}

void Room::GetRoomMemberInfoList(JCORE_OUT RoomCharacterInfo* info) {
	JCORE_LOCK_GUARD(m_Sync);
	GetRoomMemberInfoListRaw(info);
}

void Room::GetRoomMemberInfoListRaw(JCORE_OUT RoomCharacterInfo* info) {
	for (int i = 0; i < m_vPlayerList.Size(); ++i) {
		Character* pCharacter = m_vPlayerList[i]->GetCharacter();

		if (pCharacter == nullptr) {
			info[i] = {};
			continue;
		}

		pCharacter->GetRoomInfo(info[i]);
	}
}

void Room::BroadcastRoomMemberListInfo() {
	Player* pLeader = m_pLeader;

	if (pLeader == nullptr) {
		DebugAssert(false);
		return;
	}

	Character* pLeaderCharacter = pLeader->GetCharacter();

	if (pLeaderCharacter == nullptr) {
		DebugAssert(false);
		return;
	}

	JCORE_LOCK_GUARD(m_Sync);
	int iCount = m_vPlayerList.Size();
	auto pPacket = dbg_new SinglePacket<SC_UpdateRoomMemberList>(iCount);
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.HostCharacterPrimaryKey = pLeaderCharacter->GetPrimaryKey();
	GetRoomMemberInfoListRaw(pPacket->Cmd.Info);
	BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void Room::BroadcastRoomListInfo() {
	const auto vRoomInfoList = m_pLobby->GetRoomInfoList();
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

	m_pLobby->BroadcastPacket(pPacket, Const::Broadcast::Lobby::StateLobby);
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
