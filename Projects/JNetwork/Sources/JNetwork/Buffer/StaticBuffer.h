/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 6:01:50 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/Buffer/BufferAbstract.h>


NS_JNET_BEGIN

template <Int32U BufferSize>
class StaticBuffer : public BufferAbstract
{
	using TBuffer = StaticBuffer<BufferSize>;
public:
	StaticBuffer() : BufferAbstract(m_Buffer, BufferSize) {}
	~StaticBuffer() override {}

	char m_Buffer[BufferSize] = {};
};

NS_JNET_END

