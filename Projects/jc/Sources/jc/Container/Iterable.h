/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Primitives/SmartPtr.h>
#include <jc/Declspec.h>

NS_JC_BEGIN

/*=====================================================================================
					순회 가능한 = 컨테이너 최상위 인터페이스
=====================================================================================*/

// 전방 선언
template <typename, typename> class Iterator;
template <typename T, typename TAllocator>
class JC_NOVTABLE Iterable
{
public:
	using TIterator = Iterator<T, TAllocator>;

	Iterable() = default;
	virtual ~Iterable() noexcept = default;

	virtual SharedPtr<TIterator> Begin() const = 0;
	virtual SharedPtr<TIterator> End() const = 0;
};

// 스마트 포인터 적기가 힘드므로...
template <typename T, typename TAllocator = CDefaultAllocator>
using Enumerator = SharedPtr<Iterator<T, TAllocator>>;


NS_JC_END
