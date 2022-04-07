/*
 * �ۼ��� : ������
 * ä�ο��� ����� �� Ǯ�� �̸� ��������.
 * �� ä�ο� �ͼӵ� ��Ǯ�̴�.
 * �׸��� Channel���� Ǯ���� �������� ���� �� ��� ���� �������ְ� �ֱ� ������ �� Ŭ�������� ���� ���� �Ƚᵵ �ȴ�.
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
	// Ǯ���� ����� �÷��̾ ������ �����ؼ� ��ȯ�޴´�.
	bool Initialize(int size);
	bool Finalize();
	Room* PopRoom(Player* host, const JCore::String& roomName, int maxPlayerCount);

	// ������ ����� �÷��̾� ������ Ǯ�� �ٽ� �������´�.
	void ReleaseRoom(Room* releaseRoom);
	
	int Count() const;
private:
	JCore::LinkedList<Room*> m_RoomPool;
	int m_iCurRoomCount;
	void* m_pChannel;		// ��� ������ ���� void�� ��

	friend class Channel;
};

