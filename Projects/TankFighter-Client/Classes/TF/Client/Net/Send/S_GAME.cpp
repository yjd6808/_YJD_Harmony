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

