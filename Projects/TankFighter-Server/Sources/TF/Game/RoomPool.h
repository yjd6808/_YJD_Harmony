/*
 * 작성자 : 윤정도
 * 채널에서 사용할 방 풀을 미리 만들어놓자.
 * 각 채널에 귀속된 룸풀이다.
 * 그리고 Channel에서 풀에서 꺼낼때와 넣을 때 모두 락을 수행해주고 있기 떄문에 이 클래스에서 따로 락은 안써도 된다.
 */
#pragma once

#include <JCore/Container/LinkedList.h>
#include <TF/Game/Room.h>


class Channel;
class Player;
class RoomPool
{
	RoomPool(Channel* channel) : m_iCurRoomCount(0), m_pChannel(channel) {}
	Channel* GetChannel() const { return static_cast<Channel*>(m_pChannel); }
public:
	// 풀에서 사용할 플레이어에 세션을 세팅해서 반환받는다.
	bool Initialize(int size);
	bool Finalize();
	Room* PopRoom(Player* host, const JCore::String& roomName, int maxPlayerCount);

	// 연결이 종료된 플레이어 정보를 풀에 다시 돌려놓는다.
	void ReleaseRoom(Room* releaseRoom);
	
	int Count() const;
private:
	JCore::LinkedList<Room*> m_RoomPool;
	int m_iCurRoomCount;
	void* m_pChannel;		// 헤더 의존성 땜에 void로 둠

	friend class Channel;
};

