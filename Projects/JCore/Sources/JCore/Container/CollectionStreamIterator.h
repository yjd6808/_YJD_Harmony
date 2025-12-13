/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 8:42:54 AM
 * =====================
 *
 */

#pragma once

#include <JCore/Container/Iterator.h>

NS_JC_BEGIN

// 전방 선언
class CVoidOwner;
template <typename, typename> class CCollectionStream;
template <typename> struct StreamNode;

template <typename T, typename TAllocator>
class CollectionStreamIterator : public Iterator<T, TAllocator>
{
public:
	using TIterator = Iterator<T, TAllocator>;
    using TStreamNode = StreamNode<T>;
	using TCollectionStream = CCollectionStream<T, TAllocator>;

public:
	CollectionStreamIterator(CVoidOwner& _owner, TStreamNode* _current)
	: TIterator(_owner)
	{
		pCurrent_ = _current;

		TCollectionStream* pList = _owner.Get<TCollectionStream*>();
		pHead_ = pList->pHead_;
		pTail_ = pList->pTail_;
	}

	~CollectionStreamIterator() noexcept override = default;

public:
	bool HasNext() const override
	{
		if (!this->IsValid())
			return false;

		return pCurrent_ != pTail_;
	}

	bool HasPrevious() const override
	{
		if (!this->IsValid())
			return false;

		return pCurrent_ != pHead_;
	}

	T& Next() override
	{
		if (pCurrent_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		T& value = *pCurrent_->pValue_;
		pCurrent_ = pCurrent_->pNext_;
		return value;
	}

	T& Previous() override
	{
		if (pCurrent_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		T& value = *pCurrent_->pValue_;
		pCurrent_ = pCurrent_->pPrevious_;
		return value;
	}

	T& Current() override
	{
		return *pCurrent_->pValue_;
	}

	bool IsEnd() const override
	{
		return pCurrent_ == pTail_;
	}

	bool IsBegin() const override
	{
		return pCurrent_ == pHead_;
	}

protected:
	TStreamNode* pCurrent_;
	TStreamNode* pHead_;
	TStreamNode* pTail_;

	friend class TCollectionStream;
};

NS_JC_END
