/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Container/MapCollectionIterator.h>

NS_JC_BEGIN

// 전방 선언
class CVoidOwner;
template <typename> struct BucketNode;
template <typename...> struct Bucket;
template <typename, typename> class Properties;
template <typename, typename> struct Pair;

template <typename TKey, typename TAllocator>
class CPropertiesIterator : public MapCollectionIterator<TKey, PropertyBase*, TAllocator>
{
	using TBucket = Bucket<TKey, PropertyBase*, TAllocator>;
	using TKeyPropertyPair = Pair<TKey, PropertyBase*>;
	using TBucketNode = BucketNode<TKeyPropertyPair>;
	using TProperties = Properties<TKey, TAllocator>;
	using TMapCollectionIterator = MapCollectionIterator<TKey, PropertyBase*, TAllocator>;

public:
	CPropertiesIterator(CVoidOwner& _owner, TBucket* _pCurrentBucket, int _currentBucketIndex)
		: TMapCollectionIterator(_owner)
	{
		pCurrentBucket_ = _pCurrentBucket;
		currentBucketIndex_ = _currentBucketIndex;
	}

	~CPropertiesIterator() noexcept override = default;

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

	TKeyPropertyPair& Next() override
	{
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		TBucketNode& val = pCurrentBucket_->GetAt(currentBucketIndex_++);

		if (currentBucketIndex_ < pCurrentBucket_->size_)
		{
			return val.data_;
		}

		pCurrentBucket_ = pCurrentBucket_->pNext_;

		if (pCurrentBucket_)
		{
			currentBucketIndex_ = 0;
		}

		return val.data_;
	}

	TKeyPropertyPair& Previous() override
	{
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		TBucketNode& val = pCurrentBucket_->GetAt(currentBucketIndex_--);

		if (currentBucketIndex_ >= 0)
		{
			return val.data_;
		}

		pCurrentBucket_ = pCurrentBucket_->pPrevious_;

		if (pCurrentBucket_)
		{
			currentBucketIndex_ = pCurrentBucket_->size_ - 1;
		}

		return val.data_;
	}

	TKeyPropertyPair& Current() override
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
	TProperties* CastProperties() const
	{
		this->ThrowIfIteratorIsNotValid();
		return this->watcher_.template Get<TProperties*>();
	}

protected:
	int currentBucketIndex_;
	TBucket* pCurrentBucket_;

	friend class TProperties;
};

NS_JC_END
