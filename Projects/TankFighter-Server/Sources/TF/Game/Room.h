#pragma once

#include <JCore/Container/LinkedList.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Functional.h>
#include <JCore/LockGuard.h>

#include <Common/Enum.h>



/*
 * ��� ���������� ����.
 * ���� �������̽� ������ ¥�� ��� ������ ������ ���ϱ��ѵ�.. �ð��� �����ϴ�.
 *
 * #include <TF/Game/Player.h>
 * #include <TF/Game/Channel.h>
 *
 * �� 2���� ��� ������ ���� ���ϱ� ���� m_pChannel�� m_pHost�� ĳ�����ؼ� ����ϴ°ɷ�..
 */


namespace JNetwork { struct ISendPacket; }
class Player;
class Channel;
class Room
{
public:
	Room(Channel* channel, Player* host, const JCore::String& roomName, int maxPlayerCount);

	// �Һ� ������ �� ����
	Channel* GetChannel() const					{ return static_cast<Channel*>(m_pChannel); }
	JCore::String GetRoomName() const			{ return m_RoomName; }
	int GetRoomUID() const						{ return m_iRoomUID; }
	int GetMaxPlayerCount() const				{ return m_iMaxPlayerCount; }


	// ���� ����
	int GetPlayerCount();
	bool TryAddPlayer(Player* player);
	bool RemovePlayer(Player* player);
	bool ChangeHost(Player* player);
	bool ChangeNextHost();
	Player* GetHost();
	void BroadcastPacket(JNetwork::ISendPacket* packet, Player* exceptPlayer = nullptr);
	void SetRoomState(RoomState state);
	void ForEach(JCore::Action<Player*> foreachAction);
private:
	int m_iRoomUID;
	int m_iMaxPlayerCount;
	JCore::String m_RoomName;
	JCore::CriticalSectionMutex m_RoomLock;
	JCore::LinkedList<Player*> m_PlayerList;
	RoomState m_eRoomState;
	void* m_pChannel;
	void* m_pHost;

	inline static std::atomic<int> ms_iRoomSeq = 0;
};

