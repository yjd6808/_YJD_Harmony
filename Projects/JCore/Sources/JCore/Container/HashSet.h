/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Container/HashTable.h>
#include <JCore/Container/SetCollection.h>
#include <JCore/Container/HashSetIterator.h>

NS_JC_BEGIN

template <typename TKey, typename TAllocator = DefaultAllocator>
class HashSet : public SetCollection<TKey, TAllocator>
{
public:
	using THashTable			    = HashTable<TKey, TAllocator>;
	using TIterator					= Iterator<TKey, TAllocator>;
	using THashSet					= HashSet<TKey, TAllocator>;
	using THashSetIterator			= HashSetIterator<TKey, TAllocator>;
public:
	HashSet(int capacity = THashTable::ms_iTableDefaultCapacity) : Table(capacity) {}

	HashSet(const THashSet& other) : THashSet(other.Table.m_iCapacity) {
		operator=(other);
	}

	HashSet(THashSet&& other) noexcept {
		operator=(Move(other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashSet(std::initializer_list<TKey> ilist) : THashSet(ilist.size() + 1) {
		operator=(ilist);
	}

	~HashSet() noexcept override {}
public:


	THashSet& operator=(const THashSet& other) {
		Table.operator=(other.Table);
		return *this;
	}

	THashSet& operator=(THashSet&& other) noexcept {
		Table.operator=(Move(other.Table));
		return *this;
	}

	THashSet& operator=(std::initializer_list<TKey> ilist) {
		Table.operator=(ilist);
		return *this;
	}

	bool Insert(const TKey& key) override {
		return Table.Insert(key);
	}

	bool Insert(TKey&& key) override {
		return Table.Insert(Move(key));
	}

	bool Exist(const TKey& key) const override {
		return Table.Exist(key);
	}

	bool Remove(const TKey& key) override {
		return Table.Remove(key);
	}

	void Clear() noexcept override {
		Table.Clear();
	}

	typename THashTable::TBucket* Bucket(int index) const {
		return Table.Bucket();
	}

	int BucketCount() {
		return Table.BucketCount();
	}

	int Size() const override {
		return Table.Size();
	}

	bool IsEmpty() const override {
		return Table.Size() == 0;
	}

	bool ExpandIfNeeded(int size) {
		return Table.ExpandIfNeeded(size);
	}

	// ==========================================
	// 동적할당 안하고 해쉬맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		Table.ForEach(Forward<Consumer>(consumer));
	}

	// Value들만 순회해서 삭제하는 작업
	// 자주 사용해서 그냥 라이브러리에 박음
	void ForEachDelete() {
		Table.ForEachDelete();
	}

	SharedPtr<TIterator> Begin() const override {
		return MakeShared<THashSetIterator, TAllocator>(
			this->GetOwner(), 
			Table.m_pHeadBucket,
			0
		);
	}

	SharedPtr<TIterator> End() const override {
		return MakeShared<THashSetIterator, TAllocator>(
			this->GetOwner(), 
			Table.m_pTailBucket,
			Table.m_pTailBucket ? Table.m_pTailBucket->Size - 1 : -1
		);
	}

	ContainerType GetContainerType() override { return ContainerType::HashSet; }
protected:
	THashTable Table;

	friend class THashSetIterator;

}; // class HashSet<TKey, TValue>

NS_JC_END

