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

class CMemoryChunckQueue
{
	using QueueGuard = NormalLockGuard;
public:
	CMemoryChunckQueue(int _chunkSize, int _chunkCount);
	~CMemoryChunckQueue();

	void Push(void* _pChunk);
	void* Pop(OUT bool& _newAlloc);
	int FreeCount() { return chunkQueue_.Size(); }
	int TotalCount() { return totalChunkCount_; }
	int ChunkSize() { return chunkSize_; }
private:
	int chunkSize_;
	int totalChunkCount_;
	JCore::ArrayStack<void*> chunkQueue_;
	JCore::NormalLock lock_;
};



NS_JC_END
