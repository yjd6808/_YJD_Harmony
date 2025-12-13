/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollection.h>
#include <JCore/Container/ListStackIterator.h>

NS_JC_BEGIN

template <typename T, typename TAllocator = CDefaultAllocator>
class ListStack;

template <typename T, typename TAllocator>
class ListStack : public ListCollection<T, TAllocator>
{
	using TEnumerator             = Enumerator<T, TAllocator>;
	using TCollection             = Collection<T, TAllocator>;
	using TListCollection         = ListCollection<T, TAllocator>;
	using TListStack              = ListStack<T, TAllocator>;
	using TListStackIterator      = ListStackIterator<T, TAllocator>;

public:
	ListStack()
		: TListCollection()
	{
	}

	ListStack(const TListStack& _other)
		: TListCollection(_other)
	{
	}

	ListStack(TListStack&& _other) noexcept
		: TListCollection(Move(_other))
	{
	}

	ListStack(std::initializer_list<T> _ilist)
		: TListCollection(_ilist)
	{
	}

	~ListStack() noexcept override
	{
	}

public:
	TListStack& operator=(const TListStack& _other)
	{
		this->CopyFrom(_other);
		return *this;
	}

	TListStack& operator=(TListStack&& _other) noexcept
	{
		this->CopyFrom(Move(_other));
		return *this;
	}

	TListStack& operator=(std::initializer_list<T> _ilist)
	{
		this->CopyFrom(_ilist);
		return *this;
	}

	virtual void Push(const T& _data)
	{
		TListCollection::PushBack(_data);
	}

	virtual void Push(T&& _data)
	{
		TListCollection::PushBack(Move(_data));
	}

	virtual void PushAll(const TCollection& _collection)
	{
		TListCollection::PushBackAll(_collection);
	}

	template <typename... Args>
	void Emplace(Args&&... _args)
	{
		TListCollection::EmplaceBack(Forward<Args>(_args)...);
	}

	virtual void Pop()
	{
		TListCollection::PopBack();
	}

	virtual T& Top() const
	{
		return TListCollection::Back();
	}

	TEnumerator Begin() const override
	{
		return MakeShared<TListStackIterator, TAllocator>(this->GetOwner(), this->pHead_);
	}

	TEnumerator End() const override
	{
		return MakeShared<TListStackIterator, TAllocator>(this->GetOwner(), this->pTail_);
	}

	ContainerType GetContainerType() override
	{
		return ContainerType::ListStack;
	}

protected:
	friend class TListStackIterator;
};

NS_JC_END
