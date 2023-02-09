/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 6:20:03 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Pool/MemoryChuckQueue.h>

NS_JC_BEGIN

MemoryChunckQueue::MemoryChunckQueue(int chunkSize, int chunkCount)
	: m_iChunkSize(chunkSize)
	, m_iTotalChunkCount(chunkCount)
	, m_ChunkQueue(chunkCount > 0 ? chunkCount : 32)
{
	for (int i = 0; i < chunkCount; ++i) {
		m_ChunkQueue.Push(new char[chunkSize]);
	}
}

MemoryChunckQueue::~MemoryChunckQueue() {
	DebugAssertMsg(FreeCount() == m_iTotalChunkCount, "모도테코나이 메모리 청크가 있습니다.");

	while (!m_ChunkQueue.IsEmpty()) {
		DeleteArraySafe(m_ChunkQueue.Top());
		m_ChunkQueue.Pop();
	}
}

void MemoryChunckQueue::Push(void* chunk) {
	QueueGuard guard(m_Lock);
	m_ChunkQueue.Push(chunk);
	int iFreeCount = FreeCount();
	DebugAssertMsg(iFreeCount <= m_iTotalChunkCount, "알 수 없는 메모리가 스택에 포함되어있는 듯 합니다. Free가 Total보다 많네요...");
}

void* MemoryChunckQueue::Pop(Out_ bool& newAlloc) {
	QueueGuard guard(m_Lock);
	
	void* pChunk;

	if (m_ChunkQueue.IsEmpty()) {
		pChunk = new char[m_iChunkSize];
		m_iTotalChunkCount++;
		newAlloc = true;
	} else {
		pChunk = m_ChunkQueue.Top();
		m_ChunkQueue.Pop();
		newAlloc = false;
	}

	return pChunk;
}


NS_JC_END
