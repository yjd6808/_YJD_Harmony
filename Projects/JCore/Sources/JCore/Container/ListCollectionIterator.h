/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Container/Iterator.h>

namespace JCore {

		
// ���� ����
					class VoidOwner;
template <typename> class ListCollection;
template <typename> struct ListNode;
template <typename T>
class ListCollectionIterator : public Iterator<T>
{
	using TIterator		   = typename Iterator<T>;
	using TListNode		   = typename ListNode<T>;
	using TListCollection  = typename ListCollection<T>;
public:
	ListCollectionIterator(VoidOwner& owner, TListNode* current) : TIterator(owner) {
		m_pCurrent = current;

		TListCollection* pList = CastListCollection();
		m_pHead = pList->m_pHead;
		m_pTail = pList->m_pTail;
	}
	virtual ~ListCollectionIterator() noexcept {}
public:
	virtual bool HasNext() const {
		if (!this->IsValid()) {
			return false;
		}

		return m_pCurrent->Next != nullptr;		// ������ �����ߴµ� Previous�� �ع����� ��찡 ���� �� �����Ƿ�;
	}

	virtual bool HasPrevious() const {
		if (!this->IsValid()) {
			return false;
		}

		return m_pCurrent->Previous != m_pHead;

	}

	virtual T& Next() {
		// �ݺ��ڰ� �������� �����ߴµ� �����͸� �����÷��� �õ��ϴ� ���
		if (m_pCurrent == m_pTail) {
			throw InvalidOperationException("�����Ͱ� �����ϴ�.");
		}

		T& val = m_pCurrent->Value;
		m_pCurrent = m_pCurrent->Next;
		return val;
	}

	virtual T& Previous() {
		if (m_pCurrent->Previous == m_pHead) {
			throw InvalidOperationException("�����Ͱ� �����ϴ�.");
		}

		m_pCurrent = m_pCurrent->Previous;
		return m_pCurrent->Value;
	}

	virtual bool IsEnd() const {
		return m_pCurrent == m_pTail;
	}

	virtual bool IsBegin() const {
		return m_pCurrent == m_pHead->Next;
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


} // namespace JCore