/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 8:42:54 AM
 * =====================
 *
 */

#pragma once

NS_JC_BEGIN

template <typename, typename> class CollectionStream;
template <typename> struct StreamNode;

template <typename T, typename TAllocator>
class CollectionStreamIterator
{
public:
    using TStreamNode = StreamNode<T>;
	using TCollectionStream = CollectionStream<T, TAllocator>;

public:
	CollectionStreamIterator(TStreamNode* _current, TStreamNode* _pHead, TStreamNode* _pTail)
	: pCurrent_(_current)
	, pHead_(_pHead)
	, pTail_(_pTail)
	{
	}

public:
	bool HasNext() const
	{
		return pCurrent_ != pTail_;
	}

	bool HasPrevious() const
	{
		return pCurrent_ != pHead_;
	}

	T& Next()
	{
		if (pCurrent_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		T& value = *pCurrent_->pValue_;
		pCurrent_ = pCurrent_->pNext_;
		return value;
	}

	T& Previous()
	{
		if (pCurrent_ == nullptr)
			throw InvalidOperationException("데이터가 없습니다.");

		T& value = *pCurrent_->pValue_;
		pCurrent_ = pCurrent_->pPrevious_;
		return value;
	}

	T& Current()
	{
		return *pCurrent_->pValue_;
	}

	bool IsEnd() const
	{
		return pCurrent_ == pTail_;
	}

	bool IsBegin() const
	{
		return pCurrent_ == pHead_;
	}

protected:
	TStreamNode* pCurrent_;
	TStreamNode* pHead_;
	TStreamNode* pTail_;
};

NS_END