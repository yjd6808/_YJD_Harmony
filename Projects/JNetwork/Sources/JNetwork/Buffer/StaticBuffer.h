/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 6:01:50 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Buffer/Buffer.h>


NS_JNET_BEGIN

template <Int32U BufferSize>
class StaticBuffer : public Buffer
{
	using TBuffer = StaticBuffer<BufferSize>;
public:
	StaticBuffer() : Buffer(m_Buffer, BufferSize) {}

	char m_Buffer[BufferSize] = {};
};

NS_JNET_END

