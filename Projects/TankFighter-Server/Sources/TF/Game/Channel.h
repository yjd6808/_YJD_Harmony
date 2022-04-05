#pragma once

#include <JCore/Container/HashMap.h>
#include <JCore/Container/Vector.h>
#include <JCore/Functional.h>
#include <TF/Game/Room.h>

namespace JNetwork { struct ISendPacket; }

class Player;
class Channel
{
public:
	Channel(int channedUID, const JCore::String& channemName, int maxPlayerCount);

	// 불변 정보
	JCore::String GetChnnelName()	   			{ return m_ChannelName; }
	int GetChannelUID()	const 					{ return m_ChannelUID; }
	int GetMaxPlayerCount()	const				{ return m_iMaxPlayerCount; }

	// 가변 정보
	int GetPlayerCount();
	bool TryAddPlayer(Player* player);
	bool RemovePlayer(Player* player);
	bool IsPlayerExistByCharacterUID(int characterUID);
	Player* FindPlayerByCharacterUID(int characterUID);
	void PlayerForEach(JCore::Action<Player*> foreachAction);
	Player* PlayerFindIf(JCore::Func<bool, Player*> predicate);
	void BroadcastLobbyPacket(JNetwork::ISendPacket* packet);

	Room* CreateRoom(Player* creator, const JCore::String& roomName, int maxPlayerCount);
	bool RemoveRoom(const int roomUID);
	Room* GetRoom(const int roomUID);
	Room* JoinRoom(const int roomUID, Player* player);
	bool LeaveRoom(const int roomUID, Player* player);
	void RoomForEach(JCore::Action<Room*> foreachAction);
	
private:
	int m_ChannelUID;
	int m_iMaxPlayerCount;
	JCore::String m_ChannelName;
	JCore::CriticalSectionMutex m_ChannelLock;
	JCore::Vector<Player*> m_PlayerList;

	JCore::CriticalSectionMutex m_RoomMapLock;
	JCore::HashMap<int, Room*> m_RoomMap;
};

