/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:01:20 PM
 * =====================
 *
 */


#pragma once

#include <TF/Server/Contents/Room.h>

struct ChannelInfo;

class Channel : public IBroadcastable, public IUpdatable
{
public:
	Channel();
	Channel(int uid, JCore::String name, int maxPlayerCount, JCore::DateTime generatedTime);

	JCORE_GETTER(U, int, PrimaryKey, m_iPrimaryKey)
	JCORE_GETTER(U, const JCore::String&, Name, m_szName)
	JCORE_GETTER(U, int, MaxPlayerCount, m_iMaxPlayerCount)
	JCORE_GETTER(U, JCore::DateTime, GeneratedTime, m_dtGeneratedTime)

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void BroadcastPacket(JNetwork::ISendPacket* packet) override;

	ChannelInfo GetInfo();

	int AddPlayer(Player* player);
	bool RemovePlayer(Player* player);

	bool AddRoom(Room* room);
	bool RemoveRoom(Room* room);
	Room* GetRoomByName(const JCore::String& name);

	static Channel Empty;

	JCore::String ToString() const;
private:
	int m_iPrimaryKey;
	JCore::String m_szName;
	int m_iMaxPlayerCount;
	JCore::DateTime m_dtGeneratedTime;		// DB테이블에 추가된 시날짜

	JCore::NormalLock m_PlayerListLock;
	JCore::HashSet<Player*> m_hsPlayerSet;

	JCore::NormalLock m_RoomListLock;
	JCore::HashSet<Room*> m_hsRoomSet;
	JCore::HashMap<JCore::String, Room*> m_hmRoomMapByName;
};



