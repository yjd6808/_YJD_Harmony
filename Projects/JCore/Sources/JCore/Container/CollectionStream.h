/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Comparator.h>
#include <JCore/Container/CollectionType.h>
#include <JCore/Container/ContainerType.h>
#include <JCore/Container/CollectionStreamIterator.h>

NS_JC_BEGIN

template <typename, typename> class CollectionExtension;
template <typename, typename> class Vector;
template <typename, typename> class LinkedList;

template <typename T>
struct StreamNode
{
private:
	using TStreamNode = StreamNode<T>;
public:
	T* Pointer = nullptr;					// 다른 콜렉션의 데이터를 참조
	TStreamNode* Next = nullptr;
	TStreamNode* Previous = nullptr;

	T& Ref() const {
		return *Pointer;
	}
};



/*=====================================================================================
									콜렉션 스트림
           다른 콜렉션을 참조하는 물리적 배열, 논리적인 연결리스트 기반의 자료구조이다.
=====================================================================================*/

template <typename T, typename TAllocator>
class CollectionStream : public Collection<T, TAllocator>
{
	using TStreamNode		= StreamNode<T>;
	using TEnumerator		= Enumerator<T, TAllocator>;
	using TCollection		= Collection<T, TAllocator>;
	using TCollectionStream	= CollectionStream<T, TAllocator>;
	using TCollectionStreamIterator = CollectionStreamIterator<T, TAllocator>;
private: 
	// [1] : CollectionStream은 CollectionExtension 에서만 직접생성 가능하도록 한다.
	CollectionStream(TCollection* collection): TCollection() {
		m_pCollection = collection;
        int size = collection->Size();
	    this->m_iSize = size;

		if (size == 0) {
			ConnectNode(m_pHead, m_pTail);
			return;
		}

		// Memory::Allocate<TStreamNode*>(sizeof(TStreamNode) * size);
		// 물리적 배열 생성
		m_pArray = TAllocator::template Allocate<TStreamNode*>(sizeof(TStreamNode) * size, m_iAllocatedSize);

		// 논리적 연결리스트 구성
		ConnectNode(m_pHead, &m_pArray[0]);
		ConnectNode(&m_pArray[size - 1], m_pTail);

		TEnumerator it = collection->Begin();
		for (int i = 0; i < size - 1; i++) {
			// 실질적인 참조 데이터의 포인터를 담아준다.
			m_pArray[i].Pointer = AddressOf(it->Next());	

			// 초기에는 바로 다음 인덱스에 위치하는 노드가 다음 원소이므로 연결해준다.
			ConnectNode(&m_pArray[i], &m_pArray[i + 1]);		
		}

		// 마지막 원소의 참조 정보를 저장한다.
		m_pArray[size - 1].Pointer = AddressOf(it->Next());
	}

public:
	// 복사 생성 금지
	CollectionStream(const TCollectionStream& CollectionStream) = delete;
	CollectionStream(TCollectionStream&& CollectionStream) noexcept : TCollection(){
		m_pArray = CollectionStream.m_pArray;
		this->m_iSize = CollectionStream.m_iSize;
		m_pCollection = CollectionStream.m_pCollection;

		CollectionStream.m_pArray = nullptr;
		CollectionStream.m_iSize = 0;

		if (this->m_iSize == 0) {
			ConnectNode(m_pHead, m_pTail);
			return;
		}

		ConnectNode(m_pHead, CollectionStream.m_pHead->Next);
		ConnectNode(CollectionStream.m_pTail->Previous, m_pTail);
	}

	virtual ~CollectionStream() noexcept {
		if (m_pArray) TAllocator::template Deallocate(m_pArray, m_iAllocatedSize);
	}

	// TODO: 더미노드 없앨 시 수정
    TEnumerator Begin() const override {
		return MakeShared<TCollectionStreamIterator , TAllocator>(this->GetOwner(), this->m_pHead->Next);
	}

	TEnumerator End() const override {
		return MakeShared<TCollectionStreamIterator, TAllocator>(this->GetOwner(), this->m_pTail->Previous);
	}

	
public:
	ContainerType GetContainerType() override { return ContainerType::ReferenceStream; }
	CollectionType GetCollectionType() override { return CollectionType::Stream; }
	

	template <typename Consumer>
	TCollectionStream& ForEach(Consumer consumer) {
		TStreamNode* pCur = m_pHead->Next;
		while (pCur != m_pTail) {
			consumer(pCur->Ref());
			pCur = pCur->Next;
		}
		return *this;
	}

	template <typename IndexConsumer>
	TCollectionStream& ForEachWithIndex(IndexConsumer consumer) {
		TStreamNode* pCur = m_pHead->Next;
		int i = 0;
		while (pCur != m_pTail) {
			consumer(pCur->Ref(), i);
			pCur = pCur->Next;
			++i;
		}
		return *this;
	}

	template <typename TPredicate>
	TCollectionStream& Filter(TPredicate&& predicate) {
		TStreamNode* pCur = m_pHead->Next;
		int iSize = 0;
		while (pCur != m_pTail) {
			if (predicate(pCur->Ref())) {
				iSize++;
			} else {
				ConnectNode(pCur->Previous, pCur->Next);
			}
			pCur = pCur->Next;
		}

		this->m_iSize = iSize;
		return *this;
	}

	TCollectionStream& Sorted() {
		return Sorted(NaturalOrder{});
	}

	template <typename TPredicate>
	TCollectionStream& Sorted(TPredicate&& predicate) {
		MergeSort(Move(predicate));
		return *this;
	}

	T& First() const {
		DebugAssertMsg(this->m_iSize != 0, "데이터가 없습니다.");
		return *m_pHead->Next->Pointer;
	}

	T& Last() const {
		DebugAssertMsg(this->m_iSize != 0, "데이터가 없습니다.");
		return *m_pTail->Previous->Pointer;
	}


	Vector<T, TAllocator> ToVector() {
		Vector<T, TAllocator> v;
		v.PushBackAll(*this);
		return v;
	}

	LinkedList<T, TAllocator> ToLinkedList() {
		LinkedList<T, TAllocator> l;
		l.PushBackAll(*this);
		return l;
	}

protected:
	static void ConnectNode(TStreamNode* lhs, TStreamNode* rhs) {
		lhs->Next = rhs;
		rhs->Previous = lhs;
	}

	static TStreamNode* EndNode(TStreamNode* begin) {
		while (begin->Next != nullptr) {
			begin = begin->Next;
		}
		return begin;
	}

	template <typename TPredicate>
	void MergeSort(TPredicate&& predicate) {
		// 데이터가 1개 이하인 경우는 정렬 자체를 해줄 필요가 없다.
		if (this->m_iSize <= 1) {
			return;
		}

		TStreamNode* pBegin = m_pHead->Next;
		TStreamNode* pEnd = m_pTail->Previous;

		// 임시로 더미노드와 연결을 끊어준다.
		pBegin->Previous = nullptr;
		pEnd->Next = nullptr;

		TStreamNode* pSorted = MergeSort(pBegin, Move(predicate));

		if (pSorted == nullptr) {
			return;
		}

		// 다시 더미노드와 연결해준다.
		ConnectNode(m_pHead, pSorted);
		ConnectNode(EndNode(pSorted), m_pTail);
	}

	template <typename TPredicate>
	TStreamNode* MergeSort(TStreamNode* begin, TPredicate&& predicate) {
		if (begin == nullptr) {
			return nullptr;
		}

		TStreamNode* pSlow = begin;
		TStreamNode* pFast = begin;

		while (pFast != nullptr && pFast->Next != nullptr) {
			pSlow = pSlow->Next;
			pFast = pFast->Next->Next;
		}

		TStreamNode* pLeftBegin = begin;
		TStreamNode* pLeftEnd = pSlow->Previous;

		TStreamNode* pRightBegin = pSlow;

		if (pLeftBegin == pRightBegin) {
			return pLeftBegin;
		}

		if (pLeftEnd != nullptr)
			pLeftEnd->Next = nullptr;

		if (pRightBegin != nullptr)
			pRightBegin->Previous = nullptr;

		pLeftBegin = MergeSort(pLeftBegin, Move(predicate));
		pRightBegin = MergeSort(pRightBegin, Move(predicate));

		return Merge(pLeftBegin, pRightBegin, Move(predicate));
	}

	template <typename TPredicate>
	TStreamNode* Merge(TStreamNode* leftBegin, TStreamNode* rightBegin, TPredicate&& predicate) {
		TStreamNode* pTemp = &m_ValtyTemp;

		while (leftBegin != nullptr && rightBegin != nullptr) {
			if (predicate(leftBegin->Ref(), rightBegin->Ref())) {
				ConnectNode(pTemp, leftBegin);
				leftBegin = leftBegin->Next;
			} else {
				ConnectNode(pTemp, rightBegin);
				rightBegin = rightBegin->Next;
			}

			pTemp = pTemp->Next;
		}

		if (leftBegin != nullptr) {
			ConnectNode(pTemp, leftBegin);
		} else if (rightBegin != nullptr) {
			ConnectNode(pTemp, rightBegin);
		}

		m_ValtyTemp.Next->Previous = nullptr;
		return m_ValtyTemp.Next;
	}

    
	
protected:
	TCollection* m_pCollection = nullptr;
	TStreamNode* m_pArray = nullptr;
	TStreamNode* m_pHead = &m_ValtyHead;
	TStreamNode* m_pTail = &m_ValtyTail;
private:
	int m_iAllocatedSize{};

	// TODO: 추후 시간나면 더미노드 없앨 것
	TStreamNode m_ValtyHead;
	TStreamNode m_ValtyTail;
	TStreamNode m_ValtyTemp;

	friend class CollectionExtension<T, TAllocator>;
	friend class CollectionStreamIterator<T, TAllocator>;
};

NS_JC_END