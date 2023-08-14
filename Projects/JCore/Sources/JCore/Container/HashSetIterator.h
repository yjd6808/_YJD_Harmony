/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/MapCollectionIterator.h>

NS_JC_BEGIN

// 전방 선언
class VoidOwner; 
template <typename> struct Hasher;
template <typename> struct BucketNode;

template <typename...> struct Bucket;
template <typename, typename> class  HashSet;
template <typename, typename> struct Pair;

template <typename TKey, typename TAllocator>
class HashSetIterator : public SetCollectionIterator<TKey, TAllocator>
{
	using TBucket				 = Bucket<TKey, TAllocator>;
	using TBucketNode			 = BucketNode<TKey>;
	using THashSet				 = HashSet<TKey, TAllocator>;
	using TSetCollectionIterator = SetCollectionIterator<TKey, TAllocator>;
public:
	HashSetIterator(VoidOwner& owner, TBucket* currentBucket, int currentBucketIndex) : TSetCollectionIterator(owner) {
		m_pCurrentBucket = currentBucket;
		m_iCurrentBucketIndex = currentBucketIndex;
	}

	~HashSetIterator() noexcept override = default;
public:
	bool HasNext() const override {
		if (!this->IsValid())
			return false;

		if (m_pCurrentBucket != nullptr && m_iCurrentBucketIndex < m_pCurrentBucket->Size)
			return true;

		return false;
	}

	bool HasPrevious() const override {
		if (!this->IsValid()) 
			return false;

		if (m_pCurrentBucket != nullptr && m_iCurrentBucketIndex >= 0)
			return true;

		return false;
	}

	TKey& Next() override {
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		TBucketNode& val = m_pCurrentBucket->GetAt(m_iCurrentBucketIndex++);

		if (m_iCurrentBucketIndex < m_pCurrentBucket->Size) {
			return val.Data;
		}

		m_pCurrentBucket = m_pCurrentBucket->Next;

		if (m_pCurrentBucket) {
			m_iCurrentBucketIndex = 0;
		}

		return val.Data;
	}

	TKey& Previous() override {
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		TBucketNode& val = m_pCurrentBucket->GetAt(m_iCurrentBucketIndex--);

		if (m_iCurrentBucketIndex >= 0) {
			return val.Data;
		}

		m_pCurrentBucket = m_pCurrentBucket->Previous;

		if (m_pCurrentBucket) {
			m_iCurrentBucketIndex = m_pCurrentBucket->Size - 1;
		}

		return val.Data;
	}

	TKey& Current() override {
		TBucketNode& val = m_pCurrentBucket->GetAt(m_iCurrentBucketIndex);
		return val.Data;
	}

	bool IsEnd() const override {
		return HasNext() == false;
	}

	bool IsBegin() const override {
		return HasPrevious() == false;
	}

protected:
	THashSet* CastHashSet() const {
		this->ThrowIfIteratorIsNotValid();
		return this->Watcher.template Get<THashSet*>();
	}
protected:
	int m_iCurrentBucketIndex;
	TBucket* m_pCurrentBucket;
	friend class THashSet;
};

NS_JC_END