/*
	작성자 : 윤정도
*/

#pragma once


#include <JCore/Container/ArrayStackIterator.h>
#include <JCore/Container/ArrayCollection.h>

NS_JC_BEGIN

/*=====================================================================================
								배열 스택
=====================================================================================*/

template <typename T, typename TAllocator = DefaultAllocator>
class ArrayStack : public ArrayCollection<T, TAllocator>
{
	using TEnumerator			= Enumerator<T, TAllocator>;
	using TArrayCollection		= ArrayCollection<T, TAllocator>;
	using TArrayStack			= ArrayStack<T, TAllocator>;
	using TArrayStackIterator	= ArrayStackIterator<T, TAllocator>;
public:
	ArrayStack(int capacity = TArrayCollection::ms_iDefaultCapacity) : TArrayCollection(capacity) {}

	ArrayStack(int size, const T& initData) : TArrayCollection(size, initData) {}

	ArrayStack(int size, T&& initData) : TArrayCollection(size, Move(initData)) {}

	ArrayStack(const TArrayStack& other) : TArrayCollection(other) {}

	ArrayStack(TArrayStack&& other) noexcept : TArrayCollection(Move(other)) {}

	ArrayStack(std::initializer_list<T> ilist) : TArrayCollection(ilist) {}

	~ArrayStack() noexcept override {
		this->Clear(true);
	}
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
		--this->m_iSize;
	}

	template <typename... Args>
	void EmplacePush(Args&&... args) {
		if (this->IsFull()) {
			this->ExpandAuto();
		}

		this->EmplaceAt(this->m_iSize++, Forward<Args>(args)...);
	}

	TEnumerator Begin() const override {
		return MakeShared<TArrayStackIterator, TAllocator>(this->GetOwner(), 0);
	}

	TEnumerator End() const override {
		return MakeShared<TArrayStackIterator, TAllocator>(this->GetOwner(), this->Size());
	}

	ContainerType GetContainerType() override { return ContainerType::ArrayStack; }

protected:
	friend class TArrayStackIterator;
};


NS_JC_END