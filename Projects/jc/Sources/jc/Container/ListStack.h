/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/ListCollection.h"

NS_JC_BEGIN

template <typename T, typename TAllocator = CDefaultAllocator>
class ListStack;

template <typename T, typename TAllocator>
class ListStack : public ListCollection<T, TAllocator>
{
	using TListCollection			= ListCollection<T, TAllocator>;
	using TListCollectionIterator	= ListCollectionIterator<T, TAllocator, false>;
	using TListStack				= ListStack<T, TAllocator>;

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

	~ListStack() noexcept
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

	void Push(const T& _data)
	{
		TListCollection::PushBack(_data);
	}

	void Push(T&& _data)
	{
		TListCollection::PushBack(Move(_data));
	}

	template <typename TCollection>
	void PushAll(const TCollection& _collection)
	{
		TListCollection::PushBackAll(_collection);
	}

	template <typename... Args>
	void Emplace(Args&&... _args)
	{
		TListCollection::EmplaceBack(Forward<Args>(_args)...);
	}

	void Pop()
	{
		TListCollection::PopBack();
	}

	bool Pop(T* _pOut)
	{
		return TListCollection::PopBack(_pOut);
	}

	T& Top() const
	{
		return TListCollection::Back();
	}

	TListCollectionIterator Begin() const
	{
		return TListCollectionIterator(const_cast<TListCollection*>(static_cast<const TListCollection*>(this)), this->pHead_);
	}

	TListCollectionIterator End() const
	{
		return TListCollectionIterator(const_cast<TListCollection*>(static_cast<const TListCollection*>(this)), this->pTail_);
	}
};

NS_END
