/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Primitives/SmartPtr.h>
#include <JCore/Declspec.h>

NS_JC_BEGIN

/*=====================================================================================
						순회 가능한 = 컨테이너 최상위 인터페이스
=====================================================================================*/

// 전방 선언
template <typename, typename> struct Iterator;
template <typename T, typename TAllocator>
struct JCORE_NOVTABLE Iterable
{
	using TIterator = Iterator<T, TAllocator>;

	Iterable() = default;
	virtual ~Iterable() noexcept = default;

	virtual SharedPtr<TIterator> Begin() const = 0;
	virtual SharedPtr<TIterator> End() const = 0;
};

// 스마트 포인터 적기가 힘드므로...
template <typename T, typename TAllocator = DefaultAllocator>
using Enumerator = SharedPtr<Iterator<T, TAllocator>>;


NS_JC_END