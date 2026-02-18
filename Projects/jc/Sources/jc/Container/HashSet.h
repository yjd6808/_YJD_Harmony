
/*
 *	작성자 : 윤정도
 */

#pragma once

#include <jc/Container/HashTable.h>
#include <jc/Container/SetCollection.h>
#include <jc/Container/HashSetIterator.h>

NS_JC_BEGIN

template <typename TKey, typename TAllocator = CDefaultAllocator>
class HashSet : public SetCollection<TKey, TAllocator>
{
public:
	using THashTable		= HashTable<TKey, TAllocator>;
	using TIterator			= Iterator<TKey, TAllocator>;
	using THashSet			= HashSet<TKey, TAllocator>;
	using THashSetIterator	= HashSetIterator<TKey, TAllocator>;

public:
	HashSet(int _capacity = THashTable::TABLE_DEFAULT_CAPACITY)
		: table_(_capacity)
	{}

	HashSet(const THashSet& _other)
		: THashSet(_other.table_.capacity_)
	{
		operator=(_other);
	}

	HashSet(THashSet&& _other) noexcept
	{
		operator=(Move(_other));
	}

	// 이니셜라이저로 초기화 하는 경우 보통 더 확장안시킬 확률이 크므로.. 맞춤형으로 가자.
	HashSet(std::initializer_list<TKey> _ilist)
		: THashSet(static_cast<int>(_ilist.size()) + 1)
	{
		operator=(_ilist);
	}

	~HashSet() noexcept override = default;

public:
	THashSet& operator=(const THashSet& _other)
	{
		table_.operator=(_other.table_);
		return *this;
	}

	THashSet& operator=(THashSet&& _other) noexcept
	{
		table_.operator=(Move(_other.table_));
		return *this;
	}

	THashSet& operator=(std::initializer_list<TKey> _ilist)
	{
		table_.operator=(_ilist);
		return *this;
	}

	bool Insert(const TKey& _key) override
	{
		return table_.Insert(_key);
	}

	bool Insert(TKey&& _key) override
	{
		return table_.Insert(Move(_key));
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool Exist(const char* _key) const
	{
		return table_.Exist(_key);
	}

	bool Exist(const TKey& _key) const override
	{
		return table_.Exist(_key);
	}

	template <typename = DefaultEnableIf_t<IsStringType_v<TKey>>>
	bool Remove(const char* _key)
	{
		return table_.Remove(_key);
	}

	bool Remove(const TKey& _key) override
	{
		return table_.Remove(_key);
	}

	void Clear() noexcept override
	{
		table_.Clear();
	}

	typename THashTable::TBucket* Bucket(int _index) const
	{
		(void)_index;
		return table_.Bucket();
	}

	int BucketCount()
	{
		return table_.BucketCount();
	}

	int Size() const override
	{
		return table_.Size();
	}

	bool IsEmpty() const override
	{
		return table_.Size() == 0;
	}

	bool ExpandIfNeeded(int _size)
	{
		return table_.ExpandIfNeeded(_size);
	}

	// ==========================================
	// 동적할당 안하고 해쉬맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& _consumer)
	{
		table_.ForEach(Forward<Consumer>(_consumer));
	}

	// Value들만 순회해서 삭제하는 작업
	// 자주 사용해서 그냥 라이브러리에 박음
	void ForEachDelete()
	{
		table_.ForEachDelete();
	}

	SharedPtr<TIterator> Begin() const override
	{
		return MakeShared<THashSetIterator, TAllocator>(
			this->GetOwner(),
			table_.pHeadBucket_,
			0
		);
	}

	SharedPtr<TIterator> End() const override
	{
		return MakeShared<THashSetIterator, TAllocator>(
			this->GetOwner(),
			table_.pTailBucket_,
			table_.pTailBucket_ ? table_.pTailBucket_->size_ - 1 : -1
		);
	}

	ContainerType GetContainerType() override
	{
		return ContainerType::HashSet;
	}

protected:
	THashTable table_{};

	friend class THashSetIterator;
};

NS_END
