/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/MapCollectionIterator.h"

NS_JC_BEGIN

// 전방 선언
class CVoidOwner;

template <typename>
struct Hasher;

template <typename>
struct BucketNode;

template <typename...>
struct Bucket;

template <typename, typename>
class HashSet;

template <typename, typename>
struct Pair;

template <typename TKey, typename TAllocator>
class HashSetIterator : public SetCollectionIterator<TKey, TAllocator>
{
	using TBucket = Bucket<TKey, TAllocator>;
	using TBucketNode = BucketNode<TKey>;
	using THashSet = HashSet<TKey, TAllocator>;
	using TSetCollectionIterator = SetCollectionIterator<TKey, TAllocator>;

public:
	HashSetIterator(CVoidOwner& _owner, TBucket* _pCurrentBucket, int _currentBucketIndex)
		: TSetCollectionIterator(_owner)
	{
		pCurrentBucket_ = _pCurrentBucket;
		currentBucketIndex_ = _currentBucketIndex;
	}

	~HashSetIterator() noexcept override = default;

public:
	bool HasNext() const override
	{
		if (!this->IsValid())
			return false;

		if (pCurrentBucket_ != nullptr && currentBucketIndex_ < pCurrentBucket_->size_)
			return true;

		return false;
	}

	bool HasPrevious() const override
	{
		if (!this->IsValid())
			return false;

		if (pCurrentBucket_ != nullptr && currentBucketIndex_ >= 0)
			return true;

		return false;
	}

	TKey& Next() override
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

	TKey& Previous() override
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

	TKey& Current() override
	{
		TBucketNode& val = pCurrentBucket_->GetAt(currentBucketIndex_);
		return val.data_;
	}

	bool IsEnd() const override
	{
		return HasNext() == false;
	}

	bool IsBegin() const override
	{
		return HasPrevious() == false;
	}

protected:
	THashSet* CastHashSet() const
	{
		this->ThrowIfIteratorIsNotValid();
		return this->watcher_.template Get<THashSet*>();
	}

protected:
	int currentBucketIndex_;
	TBucket* pCurrentBucket_;

	friend class THashSet;
};

NS_END
