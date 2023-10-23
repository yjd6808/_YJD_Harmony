/*
 * 작성자: 윤정도
 * 생성일: 10/17/2023 9:07:02 AM
 * =====================
 *
 */


#pragma once

#include <TF/Server/Contents/Room.h>

struct RoomInfo;

class ChannelLobby : public IUpdatable, public IBroadcastable
{
public:
	ChannelLobby();
	~ChannelLobby() override;

	void BroadcastPacket(JNetwork::ISendPacket* packet, int state) override;
	void OnUpdate(const JCore::TimeSpan& elapsed) override;

	void Finalize();
	
	bool Join(Player* player);
	bool Leave(Player* player);

	bool AddPlayer(Player* player);
	bool RemovePlayer(Player* player);
	Player* GetPlayerByName(const char* name);
	void GetPlayerListByCharacterPrimaryKeyList(const JCore::Vector<int>& characterPrimaryKeyList, JCORE_REF_OUT JCore::Vector<Player*>& playerList);
	Player* GetPlayerByCharacterPrimaryKey(int characterPrimaryKey);
	
	bool AddRoom(Room* room);
	bool RemoveRoom(Room* room);

	JCore::Vector<RoomInfo> GetRoomInfoList();
	JCore::Vector<CharacterInfo> GetPlayerInfoList();

	void BroadcastRoomListInfo();
private:
	JCore::NormalLock m_PlayerListLock;
	JCore::HashSet<Player*> m_hsPlayerSet;
	JCore::HashMap<JCore::String, Player*> m_hmPlayerMapByName;
	JCore::HashMap<int, Player*> m_hmPlayerMapByCharacterPrimaryKey;

	JCore::NormalLock m_RoomListLock;
	JCore::HashSet<Room*> m_hsRoomSet;
};