/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Declspec.h>
#include <JCore/Primitives/ObserverPtr.h>

NS_JC_BEGIN

/*=====================================================================================
						반복자 최상위 인터페이스
=====================================================================================*/

// 전방 선언
template <typename, typename> class Collection;
template <typename T, typename TAllocator>
struct JCORE_NOVTABLE Iterator
{
	using TCollection = Collection<T, TAllocator>;

	Iterator(VoidOwner& owner) : Watcher(owner) {}
	virtual ~Iterator() noexcept { Watcher.~VoidWatcher(); }

	virtual bool HasNext() const = 0;
	virtual bool HasPrevious() const = 0;

	virtual bool IsEnd() const = 0;
	virtual bool IsBegin() const = 0;

	virtual T& Previous() = 0;
	virtual T& Current() = 0;
	virtual T& Next() = 0;

	virtual bool IsValid() const {
		return Watcher.Exist();
	}

protected:
	TCollection* CastCollection() const {
		ThrowIfIteratorIsNotValid();
		return Watcher.Get<TCollection>();
	}

	void ThrowIfIteratorIsNotValid() const {
		if (!IsValid()) {
			throw NullPointerException("해당 이터레이터는 더이상 존재하지 않습니다.");
		}
	}
protected:
	union { VoidWatcher Watcher; };
};

NS_JC_END