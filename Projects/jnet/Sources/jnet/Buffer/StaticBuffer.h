/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 6:01:50 PM
 * =====================
 *
 */


#pragma once

#include <jnet/Buffer/StreamBuffer.h>


NS_JNET_BEGIN

template <_u32 BufferSize>
class StaticBuffer : public StreamBuffer
{
	using TBuffer = StaticBuffer<BufferSize>;

public:
	StaticBuffer()
		: StreamBuffer(buffer_, BufferSize)
	{
	}

	~StaticBuffer() override = default;

private:
	char buffer_[BufferSize] = {};
};

NS_END
