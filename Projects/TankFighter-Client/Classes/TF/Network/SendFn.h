#pragma once

#include <string>
#include <JCore/Primitives/String.h>

struct SendFn
{
	static bool SendLoadChannelInfoSyn();
	static bool SendChannelSelectSyn(int selectedChannelUID);
	static bool SendLoadCharacterInfoSyn();
	static bool SendSelectCharacterSyn(const int selectedCharacterUID);
	static bool SendCreateCharacterSyn(std::string& nick);
	static bool SendDeleteCharacterSyn(std::string& nick);
	static bool SendJoinLobbySyn();
	static bool SendCreateRoomSyn(const JCore::String& roomName);
	static bool SendJoinRoomSyn(const int roomUID);
	static bool SendAddFriendSyn(const JCore::String& friendName);
	static bool SendDeleteFriendSyn(const int friendCharacterUID);
	static bool SendLoadRoomSyn();
	static bool SendRoomGameStartSyn(bool intrude);
	static bool SendRoomGameReadySyn();
	static bool SendRoomGameReadyCancelSyn();
	static bool SendRoomLeaveSyn();
	static bool SendBattleFieldLoadSyn();
	static bool SendBattleFieldLeaveSyn();
	static bool SendAddFriendRequestAck(int requestCharacterUID, bool accept);
	static bool SendBattileFieldTankMoveSyn(TankMove& move);
	static bool SendChatMessage(JCore::String message);
	static bool SendBattleFieldFireSyn(BulletInfo& info);
	static bool SendBattleFieldDeathSyn(int characterUID);

};

