/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Iterator.h>

NS_JC_BEGIN

// 전방 선언
class CVoidOwner;
template <typename, typename> class ListCollection;
template <typename, typename> class ListNode;

template <typename T, typename TAllocator>
class JCORE_NOVTABLE ListCollectionIterator : public Iterator<T, TAllocator>
{
	using TIterator = Iterator<T, TAllocator>;
	using TListNode = ListNode<T, TAllocator>;
	using TListCollection = ListCollection<T, TAllocator>;

public:
	ListCollectionIterator(CVoidOwner& _owner, TListNode* _pCurrent)
		: TIterator(_owner)
		, pCurrent_(_pCurrent)
	{
		TListCollection* pList = _owner.Get<TListCollection*>();
		pHead_ = pList->pHead_;
		pTail_ = pList->pTail_;
	}

	~ListCollectionIterator() noexcept override = 0;

public:
	bool HasNext() const override
	{
		if (!this->IsValid())
		{
			return false;
		}

		// 헤드까지 도달했는데 Previous를 해버리는 경우가 있을 수 있으므로
		return pCurrent_ != nullptr;
	}

	bool HasPrevious() const override
	{
		if (!this->IsValid())
		{
			return false;
		}

		return pCurrent_ != nullptr;
	}

	T& Next() override
	{
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		if (pCurrent_ == nullptr)
		{
			throw InvalidOperationException("데이터가 없습니다.");
		}

		T& value = pCurrent_->value_;
		pCurrent_ = pCurrent_->pNext_;
		return value;
	}

	T& Previous() override
	{
		if (pCurrent_ == nullptr)
		{
			throw InvalidOperationException("데이터가 없습니다.");
		}

		T& value = pCurrent_->value_;
		pCurrent_ = pCurrent_->pPrevious_;
		return value;
	}

	T& Current() override
	{
		return pCurrent_->value_;
	}

	bool IsEnd() const override
	{
		return pCurrent_ == nullptr;
	}

	bool IsBegin() const override
	{
		return pCurrent_ == pHead_;
	}

protected:
	TListCollection* CastListCollection() const
	{
		this->ThrowIfIteratorIsNotValid();
		return this->watcher_.Get<TListCollection>();
	}

protected:
	TListNode* pCurrent_;
	TListNode* pHead_;
	TListNode* pTail_;

	friend class TListCollection;
};


template <typename T, typename TAllocator>
ListCollectionIterator<T, TAllocator>::~ListCollectionIterator() noexcept
{
}


NS_JC_END
