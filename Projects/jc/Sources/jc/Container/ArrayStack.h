/*
	작성자 : 윤정도
*/

#pragma once

#include <jc/Container/ArrayStackIterator.h>
#include <jc/Container/ArrayCollection.h>

NS_JC_BEGIN

/*=====================================================================================
						배열 스택
=====================================================================================*/

template <typename T, typename TAllocator = CDefaultAllocator>
class ArrayStack final : public ArrayCollection<T, TAllocator>
{
	using TEnumerator			= Enumerator<T, TAllocator>;
	using TArrayCollection		= ArrayCollection<T, TAllocator>;
	using TArrayStack			= ArrayStack<T, TAllocator>;
	using TArrayStackIterator	= ArrayStackIterator<T, TAllocator>;

public:
	ArrayStack(int _capacity = TArrayCollection::DEFAULT_CAPACITY)
		: TArrayCollection(_capacity)
	{
	}

	ArrayStack(int _size, const T& _initData)
		: TArrayCollection(_size, _initData)
	{
	}

	ArrayStack(int _size, T&& _initData)
		: TArrayCollection(_size, Move(_initData))
	{
	}

	ArrayStack(const TArrayStack& _other)
		: TArrayCollection(_other)
	{
	}

	ArrayStack(TArrayStack&& _other) noexcept
		: TArrayCollection(Move(_other))
	{
	}

	ArrayStack(std::initializer_list<T> _ilist)
		: TArrayCollection(_ilist)
	{
	}

	~ArrayStack() noexcept override
	{
		this->Clear(true);
	}

public:
	TArrayStack& operator=(const TArrayStack& _other)
	{
		this->CopyFrom(_other);
		return *this;
	}

	TArrayStack& operator=(TArrayStack&& _other) noexcept
	{
		this->CopyFrom(Move(_other));
		return *this;
	}

	TArrayStack& operator=(std::initializer_list<T> _ilist)
	{
		this->CopyFrom(_ilist);
		return *this;
	}

	void Push(const T& _data)
	{
		if (this->IsFull())
		{
			this->ExpandAuto();
		}

		this->SetAtUnsafe(this->size_++, _data);
	}

	void Push(T&& _data)
	{
		if (this->IsFull())
		{
			this->ExpandAuto();
		}

		this->SetAtUnsafe(this->size_++, Move(_data));
	}

	T& Top() const
	{
		return this->GetAt(this->size_ - 1);
	}

	void Pop()
	{
		this->DestroyAt(this->size_ - 1);
		--this->size_;
	}

	template <typename... Args>
	void EmplacePush(Args&&... _args)
	{
		if (this->IsFull())
		{
			this->ExpandAuto();
		}

		this->EmplaceAt(this->size_++, Forward<Args>(_args)...);
	}

	TEnumerator Begin() const override
	{
		return MakeShared<TArrayStackIterator, TAllocator>(this->GetOwner(), 0);
	}

	TEnumerator End() const override
	{
		return MakeShared<TArrayStackIterator, TAllocator>(this->GetOwner(), this->Size());
	}

	ContainerType GetContainerType() override
	{
		return ContainerType::ArrayStack;
	}

protected:
	friend class ArrayStackIteratorAlias;
};

NS_END
