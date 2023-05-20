/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 2:38:40 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Sync/NormalLock.h>
#include <JCore/Container/ArrayStack.h>

NS_JC_BEGIN

class MemoryChunckQueue
{
	using QueueGuard = NormalLockGuard;
public:
	MemoryChunckQueue(int chunkSize, int chunkCount);
	~MemoryChunckQueue();

	void Push(void* chunk);
	void* Pop(JCORE_OUT bool& newAlloc);
	int FreeCount() { return m_ChunkQueue.Size();  }
	int TotalCount() { return m_iTotalChunkCount; }
	int ChunkSize() { return m_iChunkSize; }
private:
	int m_iChunkSize;
	int m_iTotalChunkCount;
	JCore::ArrayStack<void*> m_ChunkQueue;
	JCore::NormalLock m_Lock;
};



NS_JC_END

