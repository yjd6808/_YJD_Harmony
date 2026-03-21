/*
 * 작성자: 윤정도
 * 생성일: 2/8/2023 11:30:33 AM
 * =====================
 */


#pragma once

#include "jc/Pool/PooledMemoryChunk.h"

#include "jnet/Buffer/StreamBuffer.h"

NS_JNET_BEGIN

class PooledStreamBuffer : public StreamBuffer
{
public:
	PooledStreamBuffer(const jc::MemoryPoolAbstractPtr& _allocator, int _bufferSize);
	PooledStreamBuffer(const PooledStreamBuffer& _other);
	virtual ~PooledStreamBuffer() override = default;

	jc::MemoryPoolAbstractPtr GetAllocator() const { return memoryChunk_.GetAllocator(); }
	int				GetBufferRequestSize() { return memoryChunk_.GetRequestSize(); }

private:
	jc::PooledMemoryChunk memoryChunk_;
};

NS_END
