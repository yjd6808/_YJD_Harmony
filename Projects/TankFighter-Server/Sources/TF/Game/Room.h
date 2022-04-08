/*
 * �ۼ��� : ������
 */

#pragma once

#include <JCore/Container/LinkedList.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Functional.h>
#include <JCore/LockGuard.h>

#include <Common/Enum.h>

#ifndef Out_
#define Out_
#endif


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

class RoomPool;
struct RoomInfo;
class Player;
class Channel;
class Room
{
	// ����/�Ҹ��� RoomPool���� �����Ѵ�.
	Room();
	virtual ~Room() = default;

	void Initialize(Channel* channel, Player* host, const JCore::String& roomName, int maxPlayerCount);
public:
	// �Һ� ������ �� ����
	Channel* GetChannel() const					{ return static_cast<Channel*>(m_pChannel); }
	JCore::String GetRoomName() const			{ return m_RoomName; }
	int GetRoomUID() const						{ return m_iRoomUID; }
	int GetMaxPlayerCount() const				{ return m_iMaxPlayerCount; }
	void Lock()									{ m_RoomLock.Lock();}
	void Unlock()								{ m_RoomLock.Unlock();}
	

	// ���� ����
	bool IsEmpty()								{ return GetPlayerCount() == 0; }
	int GetPlayerCount();
	bool TryAddPlayer(Player* player);
	bool RemovePlayer(Player* player);
	bool ChangeHost(Player* player);
	bool ChangeNextHost();
	Player* GetHost();
	void Broadcast(JNetwork::ISendPacket* packet, Player* exceptPlayer = nullptr);
	void UnsafeBroadcast(JNetwork::ISendPacket* packet, Player* exceptPlayer = nullptr);
	void UnsafeBroadcastInBattle(JNetwork::ISendPacket* packet, Player* exceptPlayer = nullptr);
	void SetRoomState(RoomState state);
	void ForEach(JCore::Action<Player*> foreachAction);
	void UnsafeForEach(JCore::Action<Player*> foreachAction);
	RoomState GetRoomState();
	bool IsBattleFieldState();
	void LoadRoomInfo(Out_ RoomInfo& info);

	void SetTimerTime(int time);
	int SubtractTimerTime(int time);
	int AddTimerTime(int time);
	int GetTimerTime();
private:
	int m_iRoomUID;
	int m_iMaxPlayerCount;
	JCore::String m_RoomName;
	JCore::CriticalSectionMutex m_RoomLock;
	JCore::LinkedList<Player*> m_PlayerList;
	RoomState m_eRoomState;						// �� ���� : ��Ʋ ��/�κ����� ���� ��Ÿ��
	int m_iTimerTime;							// ��Ʋ���߿� ����ϴ� Ÿ�̸� ����
	void* m_pChannel;							// �� ���� ���� ä��
	void* m_pHost;								// �� ���� ����


	inline static std::atomic<int> ms_iRoomSeq = 0;

	friend class Channel;
	friend class RoomPool;
	friend class BattleFieldWorker;
};

