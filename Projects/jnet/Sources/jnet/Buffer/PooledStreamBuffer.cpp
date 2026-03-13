/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 9:56:28 PM
 * =====================
 *
 */


#include <jnet/Core.h>
#include <jnet/Buffer/PooledStreamBuffer.h>

USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
PooledStreamBuffer::PooledStreamBuffer(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize)
: memoryChunk_(_allocator, _bufferSize)
{
	buffer_ = memoryChunk_.GetBuffer();
	bufferSize_ = memoryChunk_.GetRealSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
PooledStreamBuffer::PooledStreamBuffer(const PooledStreamBuffer& _other)
: memoryChunk_(_other.memoryChunk_.GetAllocator(), _other.memoryChunk_.GetRequestSize())
{
	buffer_ = memoryChunk_.GetBuffer();
	bufferSize_ = memoryChunk_.GetRealSize();

	jc::Memory::Copy(buffer_, bufferSize_, _other.buffer_, _other.bufferSize_);

	readPos_ = _other.readPos_;
	writePos_ = _other.writePos_;
}
