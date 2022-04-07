#define _WINSOCKAPI_

#include <TF/Game/RoomPool.h>
#include <TF/Game/Channel.h>
#include <TF/Util/Console.h>

using namespace JCore;
using namespace JNetwork;


bool RoomPool::Initialize(int size) {
	for (int i = 0; i < size; i++) {
		Room* pNewRoom = new (std::nothrow) Room();

		if (pNewRoom == nullptr) {
			return false;
		}

		m_RoomPool.PushBack(pNewRoom);

	}

	m_iCurRoomCount = size;
	Console::WriteLine("[%d 채널] 룸 풀 %d 초기화완료", GetChannel()->GetChannelUID(), size);
	return true;
}

bool RoomPool::Finalize() {
	m_RoomPool.Extension().ForEach([](Room* p) { delete  p; });
	Console::WriteLine("[%d 채널] 룸 풀 비우기 완료", GetChannel()->GetChannelUID());
	return true;
}

Room* RoomPool::PopRoom(Player* host, const JCore::String& roomName, int maxPlayerCount) {
	Room* pRoom = nullptr;
	if (m_RoomPool.IsEmpty()) {
		pRoom = new Room();
		m_iCurRoomCount++;
	} else {
		pRoom = m_RoomPool.Front();
		m_RoomPool.PopFront();
	}
	pRoom->Initialize(GetChannel(), host, roomName, maxPlayerCount);
	return pRoom;
}


void RoomPool::ReleaseRoom(Room* releaseRoom) {
	m_RoomPool.PushBack(releaseRoom);
}

int RoomPool::Count() const {
	return m_RoomPool.Size();
}
