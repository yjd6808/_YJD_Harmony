/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Declspec.h>
#include <jc/Primitives/ObserverPtr.h>

NS_JC_BEGIN

/*=====================================================================================
					반복자 최상위 인터페이스
=====================================================================================*/

// 전방 선언
template <typename, typename> class Collection;
template <typename T, typename TAllocator>
class JC_NOVTABLE Iterator
{
public:
	using TCollection = Collection<T, TAllocator>;

	Iterator(CVoidOwner& _owner)
		: watcher_(_owner)
	{
	}

	virtual ~Iterator() noexcept
	{
		watcher_.~CVoidWatcher();
	}

	virtual bool HasNext() const = 0;
	virtual bool HasPrevious() const = 0;

	virtual bool IsEnd() const = 0;
	virtual bool IsBegin() const = 0;

	virtual T& Previous() = 0;
	virtual T& Current() = 0;
	virtual T& Next() = 0;

	virtual bool IsValid() const
	{
		return watcher_.Exist();
	}

protected:
	TCollection* CastCollection() const
	{
		ThrowIfIteratorIsNotValid();
		return watcher_.Get<TCollection>();
	}

	void ThrowIfIteratorIsNotValid() const
	{
		if (!IsValid())
		{
			throw NullPointerException("해당 이터레이터는 더이상 존재하지 않습니다.");
		}
	}

protected:
	union
	{
		CVoidWatcher watcher_;
	};
};

NS_JC_END
