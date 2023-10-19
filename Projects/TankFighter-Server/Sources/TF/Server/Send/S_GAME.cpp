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

void S_GAME::SEND_SC_LoadChannelInfo(const Vector<ChannelInfo>& channelInfoList) {
	const int iSize = channelInfoList.Size();
	auto sending = SendBegin<SC_LoadChannelInfo>(iSize);
	for (int i = 0; i < iSize; ++i) {
		sending.Cmd.Info[i] = channelInfoList[i];
	}
}

void S_GAME::SEND_SC_SelectChannel(int channelPrimaryKey) {
	auto sending = SendBegin<SC_SelectChannel>();
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
			info.Win = selectQryResult.Win;
			info.Lose = selectQryResult.Lose;
			info.Kill = selectQryResult.Kill;
			info.Death = selectQryResult.Death;
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

void S_GAME::SEND_SC_SelectCharacter(const CharacterInfo& info) {
	auto sending = SendBegin<SC_SelectCharacter>();
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

void S_GAME::SEND_SC_UpdateRoomListBroadcast(ChannelLobby* lobby, Room* room) {
	if (room == nullptr || lobby == nullptr)
		return;

	const auto vRoomInfoList = lobby->GetRoomInfoList();
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

	lobby->BroadcastPacket(pPacket, Const::Broadcast::Lobby::StateLobby);
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

void S_GAME::SEND_SC_AddFriendRequest(Character* character) {
	auto sending = SendBegin<SC_AddFriendRequest>();
	character->GetInfo(sending.Cmd.Info);
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

void S_GAME::SEND_SC_ChatMessageBroadcast(IBroadcastable* broadcastable, const char* msg, int state) {
	const int iMsgLen = StringUtil::LengthWithNull(msg);
	const auto pPacket = dbg_new SinglePacket<SC_ChatMessage>(iMsgLen);
	JNET_SEND_PACKET_AUTO_RELEASE_GUARD(pPacket);
	pPacket->Cmd.Message.SetStringUnsafe(msg);
	broadcastable->BroadcastPacket(pPacket, state);
}

void S_GAME::SEND_SC_UpdatePlayerListInLobby(ChannelLobby* pLobby) {
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
