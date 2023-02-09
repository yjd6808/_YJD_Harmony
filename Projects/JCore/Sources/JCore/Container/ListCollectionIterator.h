/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Iterator.h>

NS_JC_BEGIN

		
// 전방 선언
class VoidOwner;
template <typename, typename> class ListCollection;
template <typename, typename> struct ListNode;
template <typename T, typename TAllocator>
class JCORE_NOVTABLE ListCollectionIterator : public Iterator<T, TAllocator>
{
	using TIterator		   = Iterator<T, TAllocator>;
	using TListNode		   = ListNode<T, TAllocator>;
	using TListCollection  = ListCollection<T, TAllocator>;
public:
	ListCollectionIterator(VoidOwner& owner, TListNode* current) : TIterator(owner) {
		m_pCurrent = current;

		TListCollection* pList = owner.Get<TListCollection*>();
		m_pHead = pList->m_pHead;
		m_pTail = pList->m_pTail;
	}

	~ListCollectionIterator() noexcept override = 0;
public:
	bool HasNext() const override {
		if (!this->IsValid()) {
			return false;
		}

		return m_pCurrent != nullptr;		// 헤드까지 도달했는데 Previous를 해버리는 경우가 있을 수 있으므로;
	}

	bool HasPrevious() const override {
		if (!this->IsValid()) {
			return false;
		}

		return m_pCurrent != nullptr;

	}

	T& Next() override {
		// 반복자가 꼬리까지 도달했는데 데이터를 가져올려고 시도하는 경우
		if (m_pCurrent == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		T& val = m_pCurrent->Value;
		m_pCurrent = m_pCurrent->Next;
		return val;
	}

	T& Previous() override {
		if (m_pCurrent == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		T& val = m_pCurrent->Value;
		m_pCurrent = m_pCurrent->Previous;
		return val;
	}

	T& Current() override {
		return m_pCurrent->Value;
	}

	bool IsEnd() const override {
		return m_pCurrent == nullptr;
	}

	bool IsBegin() const override {
		return m_pCurrent == m_pHead;
	}
protected:
	TListCollection* CastListCollection() const {
		this->ThrowIfIteratorIsNotValid();
		return this->Watcher.Get<TListCollection>();
	}
protected:
	TListNode* m_pCurrent;
	TListNode* m_pHead;
	TListNode* m_pTail;

	friend class TListCollection;
};


template <typename T, typename TAllocator>
ListCollectionIterator<T, TAllocator>::~ListCollectionIterator() noexcept {
	
}


NS_JC_END