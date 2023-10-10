/*
 * 작성자: 윤정도
 * 생성일: 10/11/2023 6:17:36 AM
 * =====================
 * HashMapIterator과 코드 동일
 */


#pragma once


#include <JCore/Container/Iterator.h>

NS_JC_BEGIN

// 전방 선언
class VoidOwner;
struct PropertyBase;

template <typename...> class HashTable;

template <typename, typename> class  Properties;
template <typename, typename> struct Pair;

template <typename TKey, typename TAllocator>
class PropertiesIterator : Iterator<Pair<TKey, PropertyBase*>, TAllocator>
{
	using TIterator = Iterator<Pair<TKey, PropertyBase*>, TAllocator>;

	using TPair = Pair<TKey, PropertyBase*>;
	using TProperties = Properties<TKey, TAllocator>;

	using TBucket = Bucket<TKey, PropertyBase*, TAllocator>;
	using TBucketNode = BucketNode<TPair>;
public:
	PropertiesIterator(VoidOwner& owner, TBucket* currentBucket, int currentBucketIndex) : TIterator(owner) {
		m_pCurrentBucket = currentBucket;
		m_iCurrentBucketIndex = currentBucketIndex;
	}

	~PropertiesIterator() noexcept override = default;
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

	TPair& Next() override {
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

	TPair& Previous() override {
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

	TPair& Current() override {
		TBucketNode& val = m_pCurrentBucket->GetAt(m_iCurrentBucketIndex);
		return val.Data;
	}

	bool IsEnd() const override {
		return HasNext() == false;
	}

	bool IsBegin() const override {
		return HasPrevious() == false;
	}
private:
	int m_iCurrentBucketIndex;
	TBucket* m_pCurrentBucket;

	TProperties m_Properties;
};

NS_JC_END
