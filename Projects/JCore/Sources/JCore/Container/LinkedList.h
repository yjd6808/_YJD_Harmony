/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollection.h>
#include <JCore/Container/LinkedListIterator.h>

namespace JCore {

template <typename T>
class LinkedList	: public ListCollection<T>
{
	using TListNode				= typename ListNode<T>;
	using TEnumerator			= typename Enumerator<T>;
	using TCollection			= typename Collection<T>;
	using TListCollection		= typename ListCollection<T>;
	using TLinkedList			= typename LinkedList<T>;
	using TLinkedListIterator	= typename LinkedListIterator<T>;
public:
	LinkedList() 
		: TListCollection(ContainerType::LinkedList) 
	{
	}

	LinkedList(const TLinkedList& other) 
		: TListCollection(other, ContainerType::LinkedList) 
	{
	}

	LinkedList(TLinkedList&& other) 
		: TListCollection(Move(other), ContainerType::LinkedList) 
	{
	}

	LinkedList(std::initializer_list<T> ilist)
		: TListCollection(ilist, ContainerType::LinkedList) {
	}

	virtual ~LinkedList() noexcept {}
public:
	TLinkedList& operator=(const TLinkedList& other) {
		this->CopyFrom(other);
		return *this;
	}

	TLinkedList& operator=(TLinkedList&& other) {
		this->CopyFrom(Move(other));
		return *this;
	}

	TLinkedList& operator=(std::initializer_list<T> ilist) {
		this->CopyFrom(ilist);
		return *this;
	}

	virtual void PushBack(const T& data) {
		TListCollection::PushBack(data);
	}

	virtual void PushBack(T&& data) {
		TListCollection::PushBack(Move(data));
	}

	virtual void PushBackAll(const TCollection& collection) {
		TListCollection::PushBackAll(collection);
	}

	virtual void PushFront(const T& data) {
		TListCollection::PushFront(data);
	}

	virtual void PushFront(T&& data) {
		TListCollection::PushFront(Move(data));
	}

	virtual void PushFrontAll(const TCollection& collection) {
		TListCollection::PushFrontAll(collection);
	}

	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		TListCollection::EmplaceBack(Forward<Args>(args)...);
	}

	template <typename... Args>
	void EmplaceFront(Args&&... args) {
		TListCollection::EmplaceFront(Forward<Args>(args)...);
	}

	virtual T& Back() const {
		return TListCollection::Back();
	}

	virtual T& Front() const {
		return TListCollection::Front();
	}

	virtual void PopBack() {
		TListCollection::PopBack();
	}

	virtual void PopFront() {
		TListCollection::PopFront();
	}

	bool Exist(const T& data) const {
		return FindNode(data) != nullptr;
	}

	bool Remove(const T& data) {
		return TListCollection::Remove(data);
	}

	bool Remove(const TLinkedListIterator& iter) {
		return TListCollection::Remove(iter);;
	}

	template <typename Predicate>
	bool RemoveIf(Predicate predicate) {
		TListNode* pDel = TListCollection::FindNodeIf(predicate);

		if (pDel == nullptr) {
			return false;
		}

		this->ConnectNode(pDel->Previous, pDel->Next);
		delete pDel;
		this->m_iSize--;
		return true;
	}

	virtual TEnumerator Begin() const {
		return MakeShared<TLinkedListIterator>(this->GetOwner(), this->m_pHead->Next);
	}

	virtual TEnumerator End() const {
		return MakeShared<TLinkedListIterator>(this->GetOwner(), this->m_pTail);
	}


protected:
	friend class TLinkedListIterator;
	template <typename, typename> friend class HashMapIterator;
};

} // namespace JCore