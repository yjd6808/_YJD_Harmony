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
	pJoinRoomSyn->DeleteCharacterUID = friendCharacterUID;
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// LOAD_ROOM_INFO_SYN 131
bool SendFn::SendLoadRoomSyn() {
	auto* pPacket = new Packet<LoadRoomInfoSyn>();
	LoadRoomInfoSyn* pJoinRoomSyn = pPacket->Get<0>();
	pJoinRoomSyn->RoomUID = _Client->GetRoomUID();
	pJoinRoomSyn->AccountUID = _Client->GetAccountUID();
	pJoinRoomSyn->ChannelUID = _Client->GetChannelUID();
	pJoinRoomSyn->CharacterUID = _Client->GetCharacterUID();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// ROOM_GAME_START_SYN 133
bool SendFn::SendRoomGameStartSyn(bool intrude) {
	auto* pPacket = new Packet<RoomGameStartSyn>();
	RoomGameStartSyn* pRoomGameStartSyn = pPacket->Get<0>();
	pRoomGameStartSyn->RoomUID = _Client->GetRoomUID();
	pRoomGameStartSyn->AccountUID = _Client->GetAccountUID();
	pRoomGameStartSyn->ChannelUID = _Client->GetChannelUID();
	pRoomGameStartSyn->CharacterUID = _Client->GetCharacterUID();
	pRoomGameStartSyn->Intrude = intrude;
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// ROOM_GAME_READY_SYN 134
bool SendFn::SendRoomGameReadySyn() {
	auto* pPacket = new Packet<RoomGameReadySyn>();
	RoomGameReadySyn* pRoomGameReadySyn = pPacket->Get<0>();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}


// ROOM_GAME_READY_CANCEL_SYN 135
bool SendFn::SendRoomGameReadyCancelSyn() {
	auto* pPacket = new Packet<RoomGameReadyCancelSyn>();
	RoomGameReadyCancelSyn* pRoomGameReadyCancelSyn = pPacket->Get<0>();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

// ROOM_LEAVE_SYN 136
bool SendFn::SendRoomLeaveSyn() {
	auto* pPacket = new Packet<RoomLeaveSyn>();
	RoomLeaveSyn* pRoomLeaveSyn = pPacket->Get<0>();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

bool SendFn::SendBattleFieldLoadSyn() {
	auto* pPacket = new Packet<BattleFieldLoadSyn>();
	BattleFieldLoadSyn* pBattleFieldLoadSyn = pPacket->Get<0>();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

bool SendFn::SendBattleFieldLeaveSyn() {
	auto* pPacket = new Packet<BattleFieldLeaveSyn>();
	BattleFieldLeaveSyn* pBattleFieldLeaveSyn = pPacket->Get<0>();
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

bool SendFn::SendAddFriendRequestAck(int requestCharacterUID, bool accept) {
	auto* pPacket = new Packet<AddFriendRequestAck>();
	AddFriendRequestAck* pAddFriendRequestAck = pPacket->Get<0>();
	pAddFriendRequestAck->AcceptedCharacterUID = _Client->GetCharacterUID();
	pAddFriendRequestAck->RequestCharacterUID = requestCharacterUID;
	pAddFriendRequestAck->Accept = accept;
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

bool SendFn::SendBattileFieldTankMoveSyn(TankMove& move) {
	auto* pPacket = new Packet<BattileFieldTankMoveSyn>();
	BattileFieldTankMoveSyn* pBattileFieldTankMoveSyn = pPacket->Get<0>();
	Memory::CopyUnsafe(&pBattileFieldTankMoveSyn->Move, &move, sizeof(TankMove));
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

bool SendFn::SendChatMessage(JCore::String message) {
	auto* pPacket = new Packet<ChatMessageSyn>();
	ChatMessageSyn* pChatMessageSyn = pPacket->Get<0>();
	pChatMessageSyn->PlayerState = _Client->GetPlayerState();
	JCore::String msg(MESSAGE_LEN);
	msg = _Client->GetName();
	msg += " : ";
	msg += message;
	strcpy_s(pChatMessageSyn->Message, MESSAGE_LEN, msg.Source());
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}

bool SendFn::SendBattleFieldFireSyn(BulletInfo& info) {
	auto* pPacket = new Packet<BattleFieldFireSyn>();
	BattleFieldFireSyn* pBattleFieldFireSyn = pPacket->Get<0>();
	Memory::CopyUnsafe(&pBattleFieldFireSyn->BulletInfo, &info, sizeof(BulletInfo));
	bool bSendRet = _Client->SendAsync(pPacket);
	return bSendRet;
}
