#pragma once

#include <TF/Game/PlayerMap.h>
#include <JCore/Container/LinkedList.h>

class Channel;

// ������ ��ü���� ������ ��Ÿ����.
class World
{
public:
	static World* GetInstance();

	bool Initialize();
	bool Finalize();

	int GetTotalRoomCount();
	int GetTotalPlayerCount();
	int GetTotalChannelCount() const;
	Player* GetPlayerByAccountIdx(const int accountIdx);

	bool IsPlayerExist(int accountUID);

	void AddNewPlayer(Player* player);
	bool RemovePlayer(Player* player);
	Player* FindIfPlayer(JCore::Func<bool, Player*> predicate);

	int GetChannelCount() const { return m_ChannelMap.Size(); }
	Channel* GetChannel(int channelUID);
	JCore::Vector<Channel*> GetChannels() { return m_ChannelMap.Values().Extension().ToVector(); }
private:
	PlayerMap m_PlayerMap;
	JCore::HashMap<int, Channel*> m_ChannelMap;		// �Һ� - �� ���ʿ�

	inline static World* ms_pInstance = nullptr;
};


