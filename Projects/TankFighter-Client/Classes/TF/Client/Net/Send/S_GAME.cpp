/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 5:10:30 PM
 * =====================
 *
 */

#include "Pch.h"
#include "S_GAME.h"

#include <TF/Common/Command.h>

USING_NS_JC;

void S_GAME::SEND_CS_Login(const String& id, const String& pass) {
	auto sending = SendBegin<CS_Login>();
	sending.Cmd.Id = id;
	sending.Cmd.Password = pass;
}

void S_GAME::SEND_CS_Register(const JCore::String& id, const JCore::String& pass) {
	auto sending = SendBegin<CS_Register>();
	sending.Cmd.Id = id;
	sending.Cmd.Password = pass;
}

bool S_GAME::SEND_CS_LoadChannelInfo() {
	auto sending = SendBegin<CS_LoadChannelInfo>();
	return SendEndExplicit(sending);
}

void S_GAME::SEND_CS_SelectChannel(int channelPrimaryKey) {
	auto sending = SendBegin<CS_SelectChannel>();
	sending.Cmd.ChannelPrimaryKey = channelPrimaryKey;
}

bool S_GAME::SEND_CS_LoadCharacterInfo(int accountPrimaryKey, int channelPrimaryKey) {
	auto sending = SendBegin<CS_LoadCharacterInfo>();
	sending.Cmd.AccountPrimaryKey = accountPrimaryKey;
	sending.Cmd.ChannelPrimaryKey = channelPrimaryKey;
	return SendEndExplicit(sending);
}

void S_GAME::SEND_CS_LeaveChannel(int channelPrimaryKey) {
	auto sending = SendBegin<CS_LeaveChannel>();
	sending.Cmd.ChannelPrimaryKey = channelPrimaryKey;
}

void S_GAME::SEND_CS_CreateCharacter(const char* nickName) {
	auto sending = SendBegin<CS_CreateCharacter>();
	sending.Cmd.AccountPrimaryKey = Core::GameClient->GetAccountPrimaryKey();
	sending.Cmd.ChannelPrimaryKey = Core::GameClient->GetChannelPrimaryKey();
	sending.Cmd.CharacterName = nickName;
}

void S_GAME::SEND_CS_DeleteCharacter(const char* nickName) {
	auto sending = SendBegin<CS_DeleteCharacter>();
	sending.Cmd.AccountPrimaryKey = Core::GameClient->GetAccountPrimaryKey();
	sending.Cmd.ChannelPrimaryKey = Core::GameClient->GetChannelPrimaryKey();
	sending.Cmd.CharacterName = nickName;
}

void S_GAME::SEND_CS_SelectCharacter(int characterPrimaryKey) {
	auto sending = SendBegin<CS_SelectCharacter>();
	sending.Cmd.AccountPrimaryKey = Core::GameClient->GetAccountPrimaryKey();
	sending.Cmd.ChannelPrimaryKey = Core::GameClient->GetChannelPrimaryKey();
	sending.Cmd.CharacterPrimaryKey = characterPrimaryKey;
}

bool S_GAME::SEND_CS_JoinLobby() {
	auto sending = SendBegin<CS_JoinLobby>();
	sending.Cmd.AccountPrimaryKey = Core::GameClient->GetAccountPrimaryKey();
	sending.Cmd.ChannelPrimaryKey = Core::GameClient->GetChannelPrimaryKey();
	sending.Cmd.CharacterPrimaryKey = Core::GameClient->GetCharacterPrimaryKey();
	return SendEndExplicit(sending);
}

void S_GAME::SEND_CS_CreateRoom(const String& roomName) {
	auto sending = SendBegin<CS_CreateRoom>();
	sending.Cmd.RoomName = roomName;
}

void S_GAME::SEND_CS_JoinRoom(int roomAccessId) {
	auto sending = SendBegin<CS_JoinRoom>();
	sending.Cmd.RoomAccessId = roomAccessId;
}

void S_GAME::SEND_CS_AddFriend(const char* nickName) {
	auto sending = SendBegin<CS_AddFriend>();
	sending.Cmd.FriendName = nickName;
}

void S_GAME::SEND_CS_AddFriendRequest(bool accecpt, int requestCharacterAccessId) {
	auto sending = SendBegin<CS_AddFriendRequest>();
	sending.Cmd.Accept = accecpt;
	sending.Cmd.RequestCharacterAccessId = requestCharacterAccessId;
}

void S_GAME::SEND_CS_DeleteFriend(int characterPrimaryKey) {
	auto sending = SendBegin<CS_DeleteFriend>();
	sending.Cmd.DeleteCharacterPrimaryKey = characterPrimaryKey;
}

void S_GAME::SEND_CS_LeaveLobby() {
	auto sending = SendBegin<CS_LeaveLobby>();
	sending.Cmd.AccountPrimaryKey = Core::GameClient->GetAccountPrimaryKey();
	sending.Cmd.ChannelPrimaryKey = Core::GameClient->GetChannelPrimaryKey();
	sending.Cmd.CharacterPrimaryKey = Core::GameClient->GetCharacterPrimaryKey();
}

bool S_GAME::SEND_CS_LoadRoomInfo(int roomAccessId) {
	auto sending = SendBegin<CS_LoadRoomInfo>();
	sending.Cmd.RoomAccessId = roomAccessId;
	return SendEndExplicit(sending);
}

void S_GAME::SEND_CS_ChatMessage(const char* msg) {
	int iMsgLen = StringUtil::LengthWithNull(msg);
	auto sending = SendBegin<CS_ChatMessage>(iMsgLen);
	sending.Cmd.Message.SetStringUnsafe(msg);
	sending.Cmd.PlayerState = Core::GameClient->GetPlayerState();
}

