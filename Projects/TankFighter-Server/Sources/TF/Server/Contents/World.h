/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 8:59:29 PM
 * =====================
 *
 */


#pragma once

#include <TF/Server/Contents/Channel.h>

struct ChannelInfo;

class World : public IUpdatable, public IBroadcastable
{
public:
	World();

	void Initialize();
	void Finalize();

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void BroadcastPacket(JNetwork::ISendPacket* packet) override;

	int GetChannelCount() const { return m_vChannelList.Size(); }
	JCore::Vector<ChannelInfo> GetChannelInfoList();
	Channel* GetChannelByPrimaryKey(int primaryKey);

	void AddPlayer(Player* player);
	void RemovePlayer(Player* player);
	Player* GetPlayerByAccountId(const JCore::String& accountId);
private:
	JCore::HashMap<int, Channel*> m_hmChannelMapByPrimaryKey;	// Access 용도
	JCore::Vector<Channel*> m_vChannelList;						// Update 용도

	JCore::NormalLock m_PlayerListSync;
	JCore::HashSet<Player*>	m_hsPlayerList;

	JCore::NormalLock m_PlayerMapSync;
	JCore::HashMap<JCore::String, Player*> m_hmPlayerMapByAccountId;
};
