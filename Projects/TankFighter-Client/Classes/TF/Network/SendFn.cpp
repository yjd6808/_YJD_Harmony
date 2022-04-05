#include <TF/Network/SendFn.h>
#include <TF/Network/GameClient.h>
#include <Common/Command.h>

#include <cocos2d.h>

using namespace JCore;
using namespace JNetwork;
using namespace cocos2d;

#ifndef _Client
#define _Client GameClient::GetInstance()
#endif

// LOAD_CHANNEL_INFO_SYN 104
bool SendFn::SendLoadChannelInfoSyn() {
	auto* pPacket = new Packet<LoadChannelInfoSyn>();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// SELECT_CHANNEL_SYN 106
bool SendFn::SendChannelSelectSyn(int selectedChannelUID) {
	auto* pPacket = new Packet<SelectChannelSyn>();
	pPacket->Get<0>()->ChanneldUID = selectedChannelUID;
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// CREATE_CHARACTER_SYN 110
bool SendFn::SendCreateCharacterSyn(std::string& nick) {
	auto* pPacket = new Packet<CreateCharacterSyn>();
	CreateCharacterSyn* pCreateCharacterSyn = pPacket->Get<0>();
	pCreateCharacterSyn->AccountUID = _Client->GetAccountUID();
	pCreateCharacterSyn->ChannelUID = _Client->GetChannelUID();
	strcpy_s(pCreateCharacterSyn->CharacterName, NAME_LEN, nick.c_str());
	return _Client->SendAsync(pPacket);
}

// DELETE_CHARACTER_SYN 112
bool SendFn::SendDeleteCharacterSyn(std::string& nick) {
	auto* pPacket = new Packet<DeleteCharacterSyn>();
	DeleteCharacterSyn* pDeleteCharacterSyn = pPacket->Get<0>();
	pDeleteCharacterSyn->AccountUID = _Client->GetAccountUID();
	pDeleteCharacterSyn->ChannelUID = _Client->GetChannelUID();
	strcpy_s(pDeleteCharacterSyn->CharacterName, NAME_LEN, nick.c_str());
	return _Client->SendAsync(pPacket);
}

// LOAD_CHARACTER_INFO_SYN 108
bool SendFn::SendLoadCharacterInfoSyn() {
	auto* pPacket = new Packet<LoadCharacterInfoSyn>();
	LoadCharacterInfoSyn* pLoadCharacterInfoSyn = pPacket->Get<0>();
	pLoadCharacterInfoSyn->AccountUID = _Client->GetAccountUID();
	pLoadCharacterInfoSyn->ChannelUID = _Client->GetChannelUID();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// SELECT_CHARACTER_SYN 114
bool SendFn::SendSelectCharacterSyn(const int selectedCharacterUID) {
	auto* pPacket = new Packet<SelectCharacterSyn>();
	SelectCharacterSyn* pSelectCharacterSyn = pPacket->Get<0>();
	pSelectCharacterSyn->AccountUID = _Client->GetAccountUID();
	pSelectCharacterSyn->ChannelUID = _Client->GetChannelUID();
	pSelectCharacterSyn->CharacterUID = selectedCharacterUID;
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}


// JOIN_LOBBY_SYN 116
bool SendFn::SendJoinLobbySyn() {
	auto* pPacket = new Packet<JoinLobbySyn>();
	JoinLobbySyn* pJoinLobbySyn = pPacket->Get<0>();
	pJoinLobbySyn->AccountUID = _Client->GetAccountUID();
	pJoinLobbySyn->ChannelUID = _Client->GetChannelUID();
	pJoinLobbySyn->CharacterUID = _Client->GetCharacterUID();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}


// CREATE_ROOM_SYN 120
bool SendFn::SendCreateRoomSyn(const JCore::String& roomName) {
	auto* pPacket = new Packet<CreateRoomSyn>();
	CreateRoomSyn* pCreateRoomSyn = pPacket->Get<0>();
	strcpy_s(pCreateRoomSyn->RoomName, NAME_LEN, roomName.Source());
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// JOIN_ROOM_SYN 121
bool SendFn::SendJoinRoomSyn(const int roomUID) {
	auto* pPacket = new Packet<JoinRoomSyn>();
	JoinRoomSyn* pJoinRoomSyn = pPacket->Get<0>();
	pJoinRoomSyn->RoomUID = roomUID;
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// ADD_FRIEND_SYN 122
bool SendFn::SendAddFriendSyn(const JCore::String& friendName) {
	auto* pPacket = new Packet<AddFriendSyn>();
	AddFriendSyn* pAddFriendSyn = pPacket->Get<0>();
	strcpy_s(pAddFriendSyn->FriendName, NAME_LEN, friendName.Source());
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;

}

// DELETE_FRIEND_SYN 123
bool SendFn::SendDeleteFriendSyn(const int friendCharacterUID) {
	auto* pPacket = new Packet<DeleteFriendSyn>();
	DeleteFriendSyn* pJoinRoomSyn = pPacket->Get<0>();
	pJoinRoomSyn->FriendCharacterUID = friendCharacterUID;
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}
