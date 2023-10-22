/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 11:20:15 AM
 * =====================
 *
 */



#include "Pch.h"
#include "S_GAME.h"

#include <TF/Common/Command.h>

#include <TF/Server/Query/Q_GAME.h>

#include "Contents/Character.h"

USING_NS_JC;
USING_NS_JNET;

void S_GAME::SEND_SC_Login(bool success, int accountPrimaryKey) {
	auto sending = SendBegin<SC_Login>();
	sending.Cmd.Result = success;
	sending.Cmd.AccountPrimaryKey = accountPrimaryKey;
}

void S_GAME::SEND_SC_Register(bool success) {
	auto sending = SendBegin<SC_Register>();
	sending.Cmd.Result = success;
}

void S_GAME::SEND_SC_Logout() {
	auto sending = SendBegin<SC_Logout>();
}

void S_GAME::SEND_SC_LoadChannelInfo(const Vector<ChannelInfo>& channelInfoList) {
	const int iSize = channelInfoList.Size();
	auto sending = SendBegin<SC_LoadChannelInfo>(iSize);
	for (int i = 0; i < iSize; ++i) {
		sending.Cmd.Info[i] = channelInfoList[i];
	}
}

void S_GAME::SEND_SC_SelectChannel(int channelPrimaryKey) {
	auto sending = SendBegin<SC_JoinChannel>();
	sending.Cmd.ChannelPrimaryKey = channelPrimaryKey;
}

void S_GAME::SEND_SC_LeaveChannel() {
	auto sending = SendBegin<SC_LeaveChannel>();
}

void S_GAME::SEND_SC_LoadCharacterInfo(int accountPrimaryKey, int channelPrimaryKey) {
	Qry::SelectCharacterInfoListResult selectQryResult = Q_GAME::SelectCharacterInfoList(accountPrimaryKey, channelPrimaryKey);
	Vector<CharacterInfo> vCharacterInfoList(selectQryResult.RowCount);

	if (selectQryResult.HasBindedResult) {
		do {
			CharacterInfo info;
			info.PrimaryKey = selectQryResult.PrimaryKey;
			info.Name = selectQryResult.Name;
			info.WinCount = selectQryResult.Win;
			info.LoseCount = selectQryResult.Lose;
			info.KillCount = selectQryResult.Kill;
			info.DeathCount = selectQryResult.Death;
			info.Money = selectQryResult.Money;
			vCharacterInfoList.PushBack(Move(info));
		} while (selectQryResult.FetchNextRow());
	}
	
	int iCount = vCharacterInfoList.Size();
	auto sending = SendBegin<SC_LoadCharacterInfo>(iCount);
	for (int i = 0; i < iCount; ++i) {
		sending.Cmd.Info[i] = Move(vCharacterInfoList[i]);
	}
}

void S_GAME::SEND_SC_SelectCharacterAndJoinLobby(const CharacterInfo& info) {
	auto sending = SendBegin<SC_SelectCharacterAndJoinLobby>();
	sending.Cmd.info = info;
}

void S_GAME::SEND_SC_UpdateCharacterInfo(Character* character) {
	auto sending = SendBegin<SC_UpdateCharacterInfo>();
	character->GetInfo(sending.Cmd.Info);
}

void S_GAME::SEND_SC_UpdateRoomList(ChannelLobby* lobby) {
	const Vector<RoomInfo> vRoomInfoList = lobby->GetRoomInfoList();
	const int iRoomCount = vRoomInfoList.Size();

	auto sending = SendBegin<SC_UpdateRoomList>(iRoomCount);

	for (int i = 0; i < iRoomCount; ++i) {
		RoomInfo& dst = sending.Cmd.Info[i];
		const RoomInfo& src = vRoomInfoList[i];

		dst.AccessId = src.AccessId;
		dst.PlayerCount = src.PlayerCount;
		dst.MaxPlayerCount = src.MaxPlayerCount;
		dst.Name = src.Name;
		dst.RoomState = src.RoomState;
	}
}

void S_GAME::SEND_SC_UpdateFriendList(Character* character) {
	const int iFriendCount = character->GetFriendCount();
	auto sending = SendBegin<SC_UpdateFriendList>(iFriendCount);
	character->GetFriendList(sending.Cmd.Info, iFriendCount);
}

void S_GAME::SEND_SC_LeaveLobby() {
	auto sending = SendBegin<SC_LeaveLobby>();
}

void S_GAME::SEND_SC_CreateRoom(int roomAccessId) {
	auto sending = SendBegin<SC_CreateRoom>();
	sending.Cmd.RoomAccessId = roomAccessId;
}

void S_GAME::SEND_SC_JoinRoom(int roomAccessId) {
	auto sending = SendBegin<SC_JoinRoom>();
	sending.Cmd.RoomAccessId = roomAccessId;
}

void S_GAME::SEND_SC_LoadRoomInfo(Room* room) {
	auto sending = SendBegin<SC_LoadRoomInfo>();
	room->GetRoomInfo(sending.Cmd.Info);
}

void S_GAME::SEND_SC_AddFriendRequest(Character* character) {
	auto sending = SendBegin<SC_AddFriendRequest>();
	character->GetInfo(sending.Cmd.Info);
}

void S_GAME::SEND_SC_RoomGameReadyBroadcast(Room* room, int characterPrimaryKey, bool ready) {
	auto pPacket = dbg_new SinglePacket<SC_RoomGameReady>;
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.CharacterPrimaryKey = characterPrimaryKey;
	pPacket->Cmd.Ready = ready;
	room->BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void S_GAME::SEND_SC_RoomLeave() {
	auto sending = SendBegin<SC_RoomLeave>();
}

void S_GAME::SEND_SC_BattleFieldTimeSync(const TimeSpan& elapsed) {
}

void S_GAME::SEND_SC_BattleFieldSpawnTankBroadcast(Room* room, Character* character) {
}

void S_GAME::SEND_SC_BattleFieldStateChanged(RoomState state) {
}

void S_GAME::SEND_SC_ServerMessage(const char* msg) {
	const int iLen = StringUtil::LengthWithNull(msg);
	auto sending = SendBegin<SC_ServerMessage>(iLen);
	sending.Cmd.Count = iLen;
	sending.Cmd.Message.SetStringUnsafe(msg);
}

void S_GAME::SEND_SC_ServerMessage(const String& msg) {
	const int len = msg.LengthWithNull();
	auto sending = SendBegin<SC_ServerMessage>(len);
	sending.Cmd.Count = len;
	sending.Cmd.Message.SetStringUnsafe(msg);
}

bool S_GAME::SEND_SC_Disconnect(Session* session) {
	auto sending = SendBegin<SC_Disconnect>();
	SendEndExplicit(sending);
	return session->Disconnect();
}

void S_GAME::SEND_SC_ChatMessageBroadcastLobby(IBroadcastable* broadcastable, const char* msg) {
	const int iMsgLen = StringUtil::LengthWithNull(msg);
	const auto pPacket = dbg_new SinglePacket<SC_ChatMessage>(iMsgLen);
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.Message.SetStringUnsafe(msg);
	pPacket->Cmd.PlayerState = PlayerState::Lobby;
	broadcastable->BroadcastPacket(pPacket, Const::Broadcast::Lobby::StateLobby);
}

void S_GAME::SEND_SC_ChatMessageBroadcastBattleField(IBroadcastable* broadcastable, const char* msg) {
	const int iMsgLen = StringUtil::LengthWithNull(msg);
	const auto pPacket = dbg_new SinglePacket<SC_ChatMessage>(iMsgLen);
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.Message.SetStringUnsafe(msg);
	pPacket->Cmd.PlayerState = PlayerState::BattleField;
	broadcastable->BroadcastPacket(pPacket, Const::Broadcast::Room::StateAny);
}

void S_GAME::SEND_SC_UpdatePlayerListBroadcastInLobby(ChannelLobby* pLobby) {
	Vector<CharacterInfo> vInfoList = pLobby->GetPlayerInfoList(PlayerState::Lobby);
	int iCount = vInfoList.Size();
	const auto pPacket = dbg_new SinglePacket<SC_UpdatePlayerList>(iCount);
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.State = PlayerState::Lobby;
	for (int i = 0; i < iCount; ++i) {
		pPacket->Cmd.Info[i] = vInfoList[i];
	}
	pLobby->BroadcastPacket(pPacket, Const::Broadcast::Lobby::StateLobby);
}

void S_GAME::SEND_SC_UpdatePlayerList(ChannelLobby* pLobby) {
	Vector<CharacterInfo> vInfoList = pLobby->GetPlayerInfoList(PlayerState::Lobby);
	int iCount = vInfoList.Size();
	auto sending = SendBegin<SC_UpdatePlayerList>(iCount);
	sending.Cmd.State = PlayerState::Lobby;
	for (int i = 0; i < iCount; ++i) {
		sending.Cmd.Info[i] = vInfoList[i];
	}
}
