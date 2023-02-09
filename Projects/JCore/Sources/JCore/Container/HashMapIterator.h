/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/MapCollectionIterator.h>

NS_JC_BEGIN

// 전방 선언
class VoidOwner; 
template <typename> struct Hasher;
template <typename, typename, typename> class  HashMap; 
template <typename, typename, typename> struct Bucket;
template <typename, typename> struct BucketNode;
template <typename, typename> struct Pair;

template <typename TKey, typename TValue, typename TAllocator>
class HashMapIterator : public MapCollectionIterator<TKey, TValue, TAllocator>
{
	using TBucket				 = Bucket<TKey, TValue, TAllocator>;
	using TBucketNode			 = BucketNode<TKey, TValue>;
	using THashMap				 = HashMap<TKey, TValue, TAllocator>;
	using TKeyValuePair			 = Pair<TKey, TValue>;
	using TMapCollectionIterator = MapCollectionIterator<TKey, TValue, TAllocator>;
public:
	HashMapIterator(VoidOwner& owner, TBucket* currentBucket, int currentBucketIndex) : TMapCollectionIterator(owner) {
		m_pMap = CastHashMap();

		m_pCurrentBucket = currentBucket;
		m_iCurrentBucketIndex = currentBucketIndex;
	}

	~HashMapIterator() noexcept override = default;
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

	TKeyValuePair& Next() override {
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		TBucketNode& val = m_pCurrentBucket->GetAt(m_iCurrentBucketIndex++);

		if (m_iCurrentBucketIndex < m_pCurrentBucket->Size) {
			return val.Pair;
		}

		m_pCurrentBucket = m_pCurrentBucket->Next;

		if (m_pCurrentBucket) {
			m_iCurrentBucketIndex = 0;
		}

		return val.Pair;
	}

	TKeyValuePair& Previous() override {
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		TBucketNode& val = m_pCurrentBucket->GetAt(m_iCurrentBucketIndex--);

		if (m_iCurrentBucketIndex >= 0) {
			return val.Pair;
		}

		m_pCurrentBucket = m_pCurrentBucket->Previous;

		if (m_pCurrentBucket) {
			m_iCurrentBucketIndex = m_pCurrentBucket->Size - 1;
		}

		return val.Pair;
	}

	TKeyValuePair& Current() override {
		TBucketNode& val = m_pCurrentBucket->GetAt(m_iCurrentBucketIndex);
		return val.Pair;
	}

	bool IsEnd() const override {
		return HasNext() == false;
	}

	bool IsBegin() const override {
		return HasPrevious() == false;
	}

protected:
	THashMap* CastHashMap() const {
		this->ThrowIfIteratorIsNotValid();
		return this->Watcher.template Get<THashMap*>();
	}
protected:
	int m_iCurrentBucketIndex;
	TBucket* m_pCurrentBucket;
	THashMap* m_pMap;
	friend class THashMap;
};

NS_JC_END