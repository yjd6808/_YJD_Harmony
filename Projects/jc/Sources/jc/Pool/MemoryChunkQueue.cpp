/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 6:20:03 PM
 * =====================
 *
 */

#include <jc/Core.h>
#include <jc/Pool/MemoryChuckQueue.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
CMemoryChunckQueue::CMemoryChunckQueue(int _chunkSize, int _chunkCount)
	: chunkSize_(_chunkSize)
	, totalChunkCount_(_chunkCount)
	, chunkQueue_(_chunkCount > 0 ? _chunkCount : 32)
{
	for (int i = 0; i < _chunkCount; ++i)
	{
		chunkQueue_.Push(dbg_new char[_chunkSize]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
CMemoryChunckQueue::~CMemoryChunckQueue()
{
	jc_assert_msg(FreeCount() == totalChunkCount_, "모도테코나이 메모리 청크가 있습니다.");

	while (!chunkQueue_.IsEmpty())
	{
		JC_DELETE_ARRAY_SAFE(chunkQueue_.Top());
		chunkQueue_.Pop();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void CMemoryChunckQueue::Push(void* _pChunk)
{
	QueueGuard guard(lock_);
	chunkQueue_.Push(_pChunk);
	int freeCount = FreeCount();
	jc_assert_msg(freeCount <= totalChunkCount_, "알 수 없는 메모리가 스택에 포함되어있는 듯 합니다. Free가 Total보다 많네요...");
}

//////////////////////////////////////////////////////////////////////////////////////////
void* CMemoryChunckQueue::Pop(OUT bool& _newAlloc)
{
	QueueGuard guard(lock_);

	void* pChunk;

	if (chunkQueue_.IsEmpty())
	{
		pChunk = dbg_new char[chunkSize_];
		totalChunkCount_++;
		_newAlloc = true;
	}
	else
	{
		pChunk = chunkQueue_.Top();
		chunkQueue_.Pop();
		_newAlloc = false;
	}

	return pChunk;
}


NS_END
