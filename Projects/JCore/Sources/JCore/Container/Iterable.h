/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/SmartPointer.h>

namespace JCore {

/*=====================================================================================
						순회 가능한 = 컨테이너 최상위 인터페이스
=====================================================================================*/

// 전방 선언
template <typename> class Iterator;
template <typename T>
struct Iterable
{
	using TIterator = typename Iterator<T>;

	Iterable() {}
	virtual ~Iterable() noexcept {}

	virtual SharedPointer<TIterator> Begin() const = 0;
	virtual SharedPointer<TIterator> End() const = 0;
};

// 스마트 포인터 적기가 힘드므로...
template <typename T>
using Enumerator = SharedPointer<Iterator<T>>;


} // namespace JCore