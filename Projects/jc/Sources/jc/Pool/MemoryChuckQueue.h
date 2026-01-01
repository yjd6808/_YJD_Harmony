/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 2:38:40 PM
 * =====================
 *
 */


#pragma once

#include <jc/Sync/NormalLock.h>
#include <jc/Container/ArrayStack.h>

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
	jc::ArrayStack<void*> chunkQueue_;
	jc::NormalLock lock_;
};



NS_JC_END
