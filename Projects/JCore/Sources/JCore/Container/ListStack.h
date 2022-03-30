/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollection.h>
#include <JCore/Container/ListStackIterator.h>

namespace JCore {

template <typename T>
class ListStack	: public ListCollection<T>
{
	using TEnumerator			= Enumerator<T>;
	using TCollection			= Collection<T>;
	using TListCollection		= ListCollection<T>;
	using TListStack			= ListStack<T>;
	using TListStackIterator	= ListStackIterator<T>;
public:
	ListStack() 
		: TListCollection(ContainerType::ListStack) 
	{
	}

	ListStack(const TListStack& other) 
		: TListCollection(other, ContainerType::ListStack) 
	{
	}

	ListStack(TListStack&& other) noexcept
		: TListCollection(Move(other), ContainerType::ListStack) 
	{
	}

	ListStack(std::initializer_list<T> ilist)
		: TListCollection(ilist, ContainerType::ListStack)
	{
	}

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
		return MakeShared<TListStackIterator>(this->GetOwner(), this->m_pHead->Next);
	}

	TEnumerator End() const override {
		return MakeShared<TListStackIterator>(this->GetOwner(), this->m_pTail);
	}
protected:
	friend class TListStackIterator;
};

} // namespace JCore