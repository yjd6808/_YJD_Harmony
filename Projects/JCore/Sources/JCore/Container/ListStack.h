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
	using TEnumerator			= typename Enumerator<T>;
	using TCollection			= typename Collection<T>;
	using TListCollection		= typename ListCollection<T>;
	using TListStack			= typename ListStack<T>;
	using TListStackIterator	= typename ListStackIterator<T>;
public:
	ListStack() 
		: TListCollection(ContainerType::ListStack) 
	{
	}

	ListStack(const TListStack& other) 
		: TListCollection(other, ContainerType::ListStack) 
	{
	}

	ListStack(TListStack&& other) 
		: TListCollection(Move(other), ContainerType::ListStack) 
	{
	}

	ListStack(std::initializer_list<T> ilist)
		: TListCollection(ilist, ContainerType::ListStack)
	{
	}

	virtual ~ListStack() noexcept {}
public:
	TListStack& operator=(const TListStack& other) {
		this->CopyFrom(other);
		return *this;
	}

	TListStack& operator=(TListStack&& other) {
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

	virtual TEnumerator Begin() const {
		return MakeShared<TListStackIterator>(this->GetOwner(), this->m_pHead->Next);
	}

	virtual TEnumerator End() const {
		return MakeShared<TListStackIterator>(this->GetOwner(), this->m_pTail);
	}
protected:
	friend class TListStackIterator;
};

} // namespace JCore