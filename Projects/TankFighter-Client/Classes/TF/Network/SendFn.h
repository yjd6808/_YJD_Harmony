#pragma once

#include <string>

struct SendFn
{
	static void SendLoadChannelInfoSyn();
	static void SendChannelSelectSyn(int selectedChannelUID);
	static void SendLoadCharacterInfoSyn();
	static void SendSelectCharacterSyn(const int selectedCharacterUID);
	static void SendCreateCharacterSyn(std::string& nick);
	static void SendDeleteCharacterSyn(std::string& nick);
};

