/*
 * 작성자: 윤정도
 * 생성일: 2/7/2023 2:38:40 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/ArrayStack.h>

NS_JC_BEGIN

template <typename T, typename TAllocator = DefaultAllocator>
class MemoryChunckQueue : public ArrayStack <T, TAllocator>
{
};

NS_JC_END

