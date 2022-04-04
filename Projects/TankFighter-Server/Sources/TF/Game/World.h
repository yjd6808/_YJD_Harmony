#pragma once

#include <TF/Game/PlayerMap.h>


class Channel;

// 게임의 전체적인 구조를 나타낸다.
class World
{
public:
	static World* GetInstance();

	bool Initialize();
	bool Finalize();

	int GetTotalRoomCount();
	int GetTotalPlayerCount();
	int GetTotalChannelCount() const;

	bool IsPlayerExist(int accountUID);

	void AddNewPlayer(Player* player);
	bool RemovePlayer(Player* player);

	int GetChannelCount() const { return m_ChannelMap.Size(); }
	Channel* GetChannel(int channelUID);
	JCore::Vector<Channel*> GetChannels() { return m_ChannelMap.Values().Extension().ToVector(); }
private:
	PlayerMap m_PlayerMap;
	JCore::HashMap<int, Channel*> m_ChannelMap;

	inline static World* ms_pInstance = nullptr;
};


