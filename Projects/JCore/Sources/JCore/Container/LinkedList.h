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
	using TListNode				= ListNode<T>;
	using TEnumerator			= Enumerator<T>;
	using TCollection			= Collection<T>;
	using TListCollection		= ListCollection<T>;
	using TLinkedList			= LinkedList<T>;
	using TLinkedListIterator	= LinkedListIterator<T>;
public:
	LinkedList() 
		: TListCollection(ContainerType::LinkedList) 
	{
	}

	LinkedList(const TLinkedList& other) 
		: TListCollection(other, ContainerType::LinkedList) 
	{
	}

	LinkedList(TLinkedList&& other) noexcept
		: TListCollection(Move(other), ContainerType::LinkedList) 
	{
	}

	LinkedList(std::initializer_list<T> ilist)
		: TListCollection(ilist, ContainerType::LinkedList) {
	}

	~LinkedList() noexcept override {}
public:
	TLinkedList& operator=(const TLinkedList& other) {
		this->CopyFrom(other);
		return *this;
	}

	TLinkedList& operator=(TLinkedList&& other) noexcept {
		this->CopyFrom(Move(other));
		return *this;
	}

	TLinkedList& operator=(std::initializer_list<T> ilist) {
		this->CopyFrom(ilist);
		return *this;
	}

	void PushBack(const T& data) override {
		TListCollection::PushBack(data);
	}

	void PushBack(T&& data) override {
		TListCollection::PushBack(Move(data));
	}

	void PushBackAll(const TCollection& collection) override {
		TListCollection::PushBackAll(collection);
	}

	void PushFront(const T& data) override {
		TListCollection::PushFront(data);
	}

	void PushFront(T&& data) override {
		TListCollection::PushFront(Move(data));
	}

	void PushFrontAll(const TCollection& collection) override {
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

	T& Back() const override {
		return TListCollection::Back();
	}

	T& Front() const override {
		return TListCollection::Front();
	}

	void PopBack() override {
		TListCollection::PopBack();
	}

	void PopFront() override {
		TListCollection::PopFront();
	}

	bool Exist(const T& data) const {
		return this->FindNode(data) != nullptr;
	}

	bool Remove(const T& data) { 
		return TListCollection::Remove(data);
	}

	bool Remove(const TLinkedListIterator& iter) {
		return TListCollection::Remove(iter);
	}

	template <typename Predicate>
	bool RemoveIf(Predicate predicate) {
		TListNode* pDel = TListCollection::FindNodeIf(predicate);

		if (pDel == nullptr) {
			return false;
		}

		this->ConnectNode(pDel->Previous, pDel->Next);
		pDel->DeleteSelf();
		--this->m_iSize;
		return true;
	}

	TEnumerator Begin() const override {
		return MakeShared<TLinkedListIterator>(this->GetOwner(), this->m_pHead->Next);
	}

	TEnumerator End() const override {
		return MakeShared<TLinkedListIterator>(this->GetOwner(), this->m_pTail);
	}


protected:
	friend class TLinkedListIterator;
	template <typename, typename> friend class HashMapIterator;
};

} // namespace JCore