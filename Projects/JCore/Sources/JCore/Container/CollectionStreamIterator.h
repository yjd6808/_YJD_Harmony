/*
 * 작성자: 윤정도
 * 생성일: 5/8/2023 8:42:54 AM
 * =====================
 *
 */


#pragma once


#include <JCore/Container/Iterator.h>

NS_JC_BEGIN


// 전방 선언
class VoidOwner;
template <typename, typename> class CollectionStream;
template <typename> struct StreamNode;

template <typename T, typename TAllocator>
class CollectionStreamIterator : public Iterator<T, TAllocator>
{
	using TIterator = Iterator<T, TAllocator>;
	using TStreamNode = StreamNode<T>;
	using TCollectionStream = CollectionStream<T, TAllocator>;
public:
	CollectionStreamIterator(VoidOwner& owner, TStreamNode* current) : TIterator(owner) {
		m_pCurrent = current;

		TCollectionStream* pList = owner.Get<TCollectionStream*>();
		m_pHead = pList->m_pHead;
		m_pTail = pList->m_pTail;
	}

	~CollectionStreamIterator() noexcept override {}
public:
	bool HasNext() const override {
		if (!this->IsValid()) {
			return false;
		}

		return m_pCurrent != m_pTail;
	}

	bool HasPrevious() const override {
		if (!this->IsValid()) {
			return false;
		}

		return m_pCurrent != m_pHead;

	}

	T& Next() override {
		if (m_pCurrent == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		T& val = *m_pCurrent->Pointer;
		m_pCurrent = m_pCurrent->Next;
		return val;
	}

	T& Previous() override {
		if (m_pCurrent == nullptr) {
			throw InvalidOperationException("데이터가 없습니다.");
		}

		T& val = *m_pCurrent->Pointer;
		m_pCurrent = m_pCurrent->Previous;
		return val;
	}

	T& Current() override {
		return *m_pCurrent->Pointer;
	}

	bool IsEnd() const override {
		return m_pCurrent == m_pTail;
	}

	bool IsBegin() const override {
		return m_pCurrent == m_pHead;
	}
protected:
	TStreamNode* m_pCurrent;
	TStreamNode* m_pHead;
	TStreamNode* m_pTail;

	friend class TCollectionStream;
};

NS_JC_END

