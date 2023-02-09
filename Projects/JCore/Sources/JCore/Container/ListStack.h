/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollection.h>
#include <JCore/Container/ListStackIterator.h>

NS_JC_BEGIN

template <typename T, typename TAllocator = DefaultAllocator>
class ListStack;

template <typename T, typename TAllocator>
class ListStack	: public ListCollection<T, TAllocator>
{
	using TEnumerator			= Enumerator<T, TAllocator>;
	using TCollection			= Collection<T, TAllocator>;
	using TListCollection		= ListCollection<T, TAllocator>;
	using TListStack			= ListStack<T, TAllocator>;
	using TListStackIterator	= ListStackIterator<T, TAllocator>;
public:
	ListStack() : TListCollection() {}

	ListStack(const TListStack& other) : TListCollection(other) {}

	ListStack(TListStack&& other) noexcept : TListCollection(Move(other)) {}

	ListStack(std::initializer_list<T> ilist) : TListCollection(ilist) {}

	~ListStack() noexcept override {}
public:
	TListStack& operator=(const TListStack& other) {
		this->CopyFrom(other);
		return *this;
	}

	TListStack& operator=(TListStack&& other) noexcept {
		this->CopyFrom(Move(other));
		return *this;
	}

	TListStack& operator=(std::initializer_list<T> ilist) {
		this->CopyFrom(ilist);
		return *this;
	}

	virtual void Push(const T& data) {
		TListCollection::PushBack(data);
	}

	virtual void Push(T&& data) {
		TListCollection::PushBack(Move(data));
	}

	virtual void PushAll(const TCollection& collection) {
		TListCollection::PushBackAll(collection);
	}

	template <typename... Args>
	void Emplace(Args&&... args) {
		TListCollection::EmplaceBack(Forward<Args>(args)...);
	}

	virtual void Pop() {
		TListCollection::PopBack();
	}

	virtual T& Top() const {
		return TListCollection::Back();
	}

	TEnumerator Begin() const override {
		return MakeShared<TListStackIterator, TAllocator>(this->GetOwner(), this->m_pHead);
	}

	TEnumerator End() const override {
		return MakeShared<TListStackIterator, TAllocator>(this->GetOwner(), this->m_pTail);
	}

	ContainerType GetContainerType() override { return ContainerType::ListStack; }
protected:
	friend class TListStackIterator;
};

NS_JC_END