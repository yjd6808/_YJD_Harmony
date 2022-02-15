/*
	�ۼ��� : ������
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
	LinkedList() : TListCollection() {}
	virtual ~LinkedList() noexcept {}
public:
	virtual void PushBack(const T& data) {
		TListCollection::PushBack(data);
	}

	virtual void PushBack(T&& data) {
		TListCollection::PushBack(Move(data));
	}

	virtual void PushBackAll(const TCollection& collection) {
		TListCollection::PushBackAll(collection);
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

	bool Find(const T& data) const {
		return FindNode(data) != nullptr;
	}

	bool Remove(const T& data) {
		TListNode* pDel = FindNode(data);

		if (pDel == nullptr) {
			return false;
		}

		this->Connect(pDel->Previous, pDel->Next);
		delete pDel;
		return true;
	}

	virtual TEnumerator Begin() const {
		return MakeShared<TLinkedListIterator>(this->GetOwner(), this->m_pHead->Next);
	}

	virtual TEnumerator End() const {
		return MakeShared<TLinkedListIterator>(this->GetOwner(), this->m_pTail);
	}
protected:
	TListNode* FindNode(const T& data) const {
		TListNode* pCur = this->m_pHead->Next;
		while (pCur != this->m_pTail) {
			if (pCur->Value == data) {
				return pCur;
			}
		}

		return nullptr;
	}

protected:
	friend class TLinkedListIterator;
};

} // namespace JCore