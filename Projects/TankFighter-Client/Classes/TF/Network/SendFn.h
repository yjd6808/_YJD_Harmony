#pragma once

#include <string>
#include <JCore/String.h>

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
};

