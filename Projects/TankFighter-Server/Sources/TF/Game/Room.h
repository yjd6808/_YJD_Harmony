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
 * 헤더 의존성땜에 뺐다.
 * 원래 인터페이스 구조로 짜면 헤더 의존성 관리가 편하긴한데.. 시간이 부족하다.
 *
 * #include <TF/Game/Player.h>
 * #include <TF/Game/Channel.h>
 *
 * 이 2개의 헤더 파일을 참조 못하기 땜에 m_pChannel과 m_pHost는 캐스팅해서 사용하는걸로..
 */


namespace JNetwork { struct ISendPacket; }
struct RoomInfo;
class Player;
class Channel;
class Room
{
public:
	Room(Channel* channel, Player* host, const JCore::String& roomName, int maxPlayerCount);

	// 불변 정보는 락 안함
	Channel* GetChannel() const					{ return static_cast<Channel*>(m_pChannel); }
	JCore::String GetRoomName() const			{ return m_RoomName; }
	int GetRoomUID() const						{ return m_iRoomUID; }
	int GetMaxPlayerCount() const				{ return m_iMaxPlayerCount; }
	void Lock()									{ m_RoomLock.Lock();}
	void Unlock()								{ m_RoomLock.Unlock();}

	// 가변 정보
	bool IsEmpty()								{ return GetPlayerCount() == 0; }
	int GetPlayerCount();
	bool TryAddPlayer(Player* player);
	bool RemovePlayer(Player* player);
	bool ChangeHost(Player* player);
	bool ChangeNextHost();
	Player* GetHost();
	void BroadcastPacket(JNetwork::ISendPacket* packet, Player* exceptPlayer = nullptr);
	void SetRoomState(RoomState state);
	void ForEach(JCore::Action<Player*> foreachAction);

	void LoadRoomInfo(Out_ RoomInfo& info);
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
	friend class Channel;
};

