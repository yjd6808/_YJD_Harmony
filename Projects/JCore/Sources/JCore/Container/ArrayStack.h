/*
	작성자 : 윤정도
*/

#pragma once


#include <JCore/Container/ArrayStackIterator.h>
#include <JCore/Container/ArrayCollection.h>

namespace JCore {

/*=====================================================================================
								배열 스택
=====================================================================================*/

template <typename T>
class ArrayStack : public ArrayCollection<T>
{
	using TEnumerator			= Enumerator<T>;
	using TArrayCollection		= ArrayCollection<T>;
	using TArrayStack			= ArrayStack<T>;
	using TArrayStackIterator	= ArrayStackIterator<T>;
public:
	ArrayStack(int capacity = TArrayCollection::ms_iDefaultCapacity) 
		: TArrayCollection(capacity, ContainerType::ArrayStack) 
	{
	}

	ArrayStack(const TArrayStack& other) 
		: TArrayCollection(other, ContainerType::ArrayStack) 
	{
	}

	ArrayStack(TArrayStack&& other) noexcept
		: TArrayCollection(Move(other), ContainerType::ArrayStack) 
	{
	}
	ArrayStack(std::initializer_list<T> ilist) 
		: TArrayCollection(ilist, ContainerType::ArrayStack) 
	{
	}

	~ArrayStack() noexcept override {}
public:
	TArrayStack& operator=(const TArrayStack& other) {
		this->CopyFrom(other);
		return *this;
	}

	TArrayStack& operator=(TArrayStack&& other) noexcept {
		this->CopyFrom(Move(other));
		return *this;
	}

	TArrayStack& operator=(std::initializer_list<T> ilist) {
		this->CopyFrom(ilist);
		return *this;
	}

	void Push(const T& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAtUnsafe(this->m_iSize++, data);
	}

	void Push(const T&& data) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->SetAtUnsafe(this->m_iSize++, Move(data));
	}

	T& Top() const {
		return this->GetAt(this->m_iSize - 1);
	}

	void Pop() {
		this->DestroyAt(this->m_iSize - 1);
		this->m_iSize--;
	}

	template <typename... Args>
	void EmplacePush(Args&&... args) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->EmplaceAt(this->m_iSize++, Forward<Args>(args)...);
	}

	TEnumerator Begin() const override {
		return MakeShared<TArrayStackIterator>(this->GetOwner(), 0);
	}

	TEnumerator End() const override {
		return MakeShared<TArrayStackIterator>(this->GetOwner(), this->Size());
	}

protected:
	friend class TArrayStackIterator;
};



} // namespace JCore