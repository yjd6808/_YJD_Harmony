/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollection.h>
#include <JCore/Container/LinkedListIterator.h>

NS_JC_BEGIN

template <typename T, typename TAllocator = DefaultAllocator>
class LinkedList	: public ListCollection<T, TAllocator>
{
	using TListNode				= ListNode<T, TAllocator>;
	using TEnumerator			= Enumerator<T, TAllocator>;
	using TCollection			= Collection<T, TAllocator>;
	using TListCollection		= ListCollection<T, TAllocator>;
	using TLinkedList			= LinkedList<T, TAllocator>;
	using TLinkedListIterator	= LinkedListIterator<T, TAllocator>;
public:
	LinkedList() : TListCollection() {}

	LinkedList(const TLinkedList& other) : TListCollection(other) {}

	LinkedList(TLinkedList&& other) noexcept : TListCollection(Move(other)) {}

	LinkedList(std::initializer_list<T> ilist) : TListCollection(ilist) {}

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

	/*void PushFront(const T& data) override {
		TListCollection::PushFront(data);
	}*/

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

	template <typename TPredicate>
	bool RemoveIf(TPredicate&& predicate) {
		TListNode* pDel = TListCollection::FindNodeIf(Move(predicate));

		if (pDel == nullptr) {
			return false;
		}

		this->RemoveNode(pDel);
		pDel->DeleteSelf();
		--this->m_iSize;
		return true;
	}

	TEnumerator Begin() const override {
		return MakeShared<TLinkedListIterator, TAllocator>(this->GetOwner(), this->m_pHead);
	}

	TEnumerator End() const override {
		return MakeShared<TLinkedListIterator, TAllocator>(this->GetOwner(), this->m_pTail);
	}

	ContainerType GetContainerType() override { return ContainerType::LinkedList; }
protected:
	friend class TLinkedListIterator;
	template <typename, typename, typename> friend class HashMapIterator;
};

NS_JC_END