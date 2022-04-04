#pragma once

#include <JCore/Container/HashMap.h>
#include <TF/Game/Player.h>

class Channel
{
public:
	Channel(int channedUID, const JCore::String& channemName, int maxPlayerCount);

	JCore::String GetChnnelName();
	int GetChannelUID();
	int GetPlayerCount();
	int GetMaxPlayerCount();
	bool TryAddPlayer(Player* player);
	bool RemovePlayer(Player* player);
private:
	int m_ChannelUID;
	int m_iMaxPlayerCount;
	int m_iPlayerCount;
	JCore::String m_ChannelName;
	JCore::CriticalSectionMutex m_ChannelLock;
	JCore::Vector<Player*> m_PlayerList;
};

