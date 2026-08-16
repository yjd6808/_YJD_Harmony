/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/HashTable.h"

NS_JC_BEGIN

template <typename TKey, typename TAllocator>
class HashSetIterator
{
	using TBucket = Bucket<TKey, TAllocator>;
	using TBucketNode = BucketNode<TKey>;

public:
	HashSetIterator(TBucket* _pCurrentBucket, int _currentBucketIndex)
		: pCurrentBucket_(_pCurrentBucket)
		, currentBucketIndex_(_currentBucketIndex)
	{
	}

	bool HasNext() const
	{
		if (pCurrentBucket_ != nullptr && currentBucketIndex_ < pCurrentBucket_->size_)
			return true;

		return false;
	}

	bool HasPrevious() const
	{
		if (pCurrentBucket_ != nullptr && currentBucketIndex_ >= 0)
			return true;

		return false;
	}

	TKey& Next()
	{
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		TBucketNode& val = pCurrentBucket_->GetAt(currentBucketIndex_++);

		if (currentBucketIndex_ < pCurrentBucket_->size_)
			return val.data_;

		pCurrentBucket_ = pCurrentBucket_->pNext_;

		if (pCurrentBucket_)
			currentBucketIndex_ = 0;

		return val.data_;
	}

	TKey& Previous()
	{
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		TBucketNode& val = pCurrentBucket_->GetAt(currentBucketIndex_--);

		if (currentBucketIndex_ >= 0)
			return val.data_;

		pCurrentBucket_ = pCurrentBucket_->pPrevious_;

		if (pCurrentBucket_)
			currentBucketIndex_ = pCurrentBucket_->size_ - 1;

		return val.data_;
	}

	TKey& Current()
	{
		TBucketNode& val = pCurrentBucket_->GetAt(currentBucketIndex_);
		return val.data_;
	}

	bool IsEnd() const
	{
		return HasNext() == false;
	}

	bool IsBegin() const
	{
		return HasPrevious() == false;
	}

private:
	int currentBucketIndex_;
	TBucket* pCurrentBucket_;
};

NS_END