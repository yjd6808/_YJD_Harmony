/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Container/Collection.h>

#include <JCore/Memory.h>
#include <JCore/Comparator.h>

namespace JCore {

template <typename> class CollectionExtension;

template <typename T>
struct StreamNode
{
private:
	using TStreamNode = typename StreamNode<T>;
public:
	T* Pointer = nullptr;					// �ٸ� �ݷ����� �����͸� ����
	TStreamNode* Next = nullptr;
	TStreamNode* Previous = nullptr;

	T& Ref() const {
		return *Pointer;
	}
};





/*=====================================================================================
									�ݷ��� ��Ʈ��
           �ٸ� �ݷ����� �����ϴ� ������ �迭, ������ ���Ḯ��Ʈ ����� �ڷᱸ���̴�.
=====================================================================================*/

template <typename T>
class CollectionStream
{
	using TEnumerator			= typename Enumerator<T>;
	using TStreamNode			= typename StreamNode<T>;
	using TCollection			= typename Collection<T>;
	using TCollectionStream		= typename CollectionStream<T>;

private: 
	// [1] : CollectionStream�� CollectionExtension ������ �������� �����ϵ��� �Ѵ�.
	CollectionStream(TCollection* collection)  {
		m_pCollection = collection;
		m_iSize = collection->Size();

		if (m_iSize == 0) {
			ConnectNode(m_pHead, m_pTail);
			return;
		}

		// ������ �迭 ����
		m_pArray = Memory::Allocate<TStreamNode*>(sizeof(TStreamNode) * m_iSize);

		// ���� ���Ḯ��Ʈ ����
		ConnectNode(m_pHead, &m_pArray[0]);
		ConnectNode(&m_pArray[m_iSize - 1], m_pTail);

		TEnumerator it = collection->Begin();
		for (int i = 0; i < m_iSize - 1; i++) {
			// �������� ���� �������� �����͸� ����ش�.
			m_pArray[i].Pointer = AddressOf(it->Next());	

			// �ʱ⿡�� �ٷ� ���� �ε����� ��ġ�ϴ� ��尡 ���� �����̹Ƿ� �������ش�.
			ConnectNode(&m_pArray[i], &m_pArray[i + 1]);		
		}

		// ������ ������ ���� ������ �����Ѵ�.
		m_pArray[m_iSize - 1].Pointer = AddressOf(it->Next());

		
		/*
		������ �� �� ������
		for (int i = 0; i < m_iSize - 1; i++) {
			ConnectNode(&m_pArray[i], &m_pArray[i + 1]);
		}

		TEnumerator it = collection->Begin();
		for (int i = 0; it->HasNext(); i++) {
			m_pArray[i].Pointer = AddressOf(it->Next());
		}
		*/
	}

public:
	// ���� ���� ����
	CollectionStream(const TCollectionStream& CollectionStream) = delete;
	CollectionStream(TCollectionStream&& CollectionStream) {
		m_pArray = CollectionStream.m_pArray;
		m_iSize = CollectionStream.m_iSize;
		m_pCollection = CollectionStream.m_pCollection;

		CollectionStream.m_pArray = nullptr;
		CollectionStream.m_iSize = 0;

		if (m_iSize == 0) {
			ConnectNode(m_pHead, m_pTail);
			return;
		}

		ConnectNode(m_pHead, CollectionStream.m_pHead->Next);
		ConnectNode(CollectionStream.m_pTail->Previous, m_pTail);
	}

	virtual ~CollectionStream() noexcept {
		Memory::Deallocate(m_pArray);
	}
public:
	int Size() const {
		return m_iSize;
	}

	template <typename Consumer>
	void ForEach(Consumer consumer) const {
		TStreamNode* pCur = m_pHead->Next;
		while (pCur != m_pTail) {
			consumer(pCur->Ref());
			pCur = pCur->Next;
		}
	}

	template <typename Predicate>
	TCollectionStream& Filter(Predicate predicate) {
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

		m_iSize = iSize;
		return *this;
	}

	TCollectionStream& Sorted() {
		return Sorted(NaturalOrder{});
	}

	template <typename Predicate>
	TCollectionStream& Sorted(Predicate predicate) {
		MergeSort(predicate);
		return *this;
	}

	T& First() const {
		this->ThrowIfNoElements();
		return *m_pHead->Next->Pointer;
	}

	T& Last() const {
		this->ThrowIfNoElements();
		return *m_pTail->Previous->Pointer;
	}

protected:
	void ConnectNode(TStreamNode* lhs, TStreamNode* rhs) {
		lhs->Next = rhs;
		rhs->Previous = lhs;
	}

	TStreamNode* EndNode(TStreamNode* begin) const {
		while (begin->Next != nullptr) {
			begin = begin->Next;
		}
		return begin;
	}

	template <typename Predicate>
	void MergeSort(Predicate predicate) {
		// �����Ͱ� 1�� ������ ���� ���� ��ü�� ���� �ʿ䰡 ����.
		if (m_iSize <= 1) {
			return;
		}

		TStreamNode* pBegin = m_pHead->Next;
		TStreamNode* pEnd = m_pTail->Previous;

		// �ӽ÷� ���̳��� ������ �����ش�.
		pBegin->Previous = nullptr;
		pEnd->Next = nullptr;

		TStreamNode* pSorted = MergeSort(pBegin, predicate);

		if (pSorted == nullptr) {
			return;
		}

		// �ٽ� ���̳��� �������ش�.
		ConnectNode(m_pHead, pSorted);
		ConnectNode(EndNode(pSorted), m_pTail);
	}

	template <typename Predicate>
	TStreamNode* MergeSort(TStreamNode* begin, Predicate predicate) {
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

		pLeftBegin = MergeSort(pLeftBegin, predicate);
		pRightBegin = MergeSort(pRightBegin, predicate);

		return Merge(pLeftBegin, pRightBegin, predicate);
	}

	template <typename Predicate>
	TStreamNode* Merge(TStreamNode* leftBegin, TStreamNode* rightBegin, Predicate predicate) {
		TStreamNode* pTemp = &_ValtyTemp;

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

		_ValtyTemp.Next->Previous = nullptr;
		return _ValtyTemp.Next;
	}

	

	
protected:
	TCollection* m_pCollection = nullptr;
	TStreamNode* m_pArray = nullptr;
	TStreamNode* m_pHead = &_ValtyHead;
	TStreamNode* m_pTail = &_ValtyTail;
	int m_iSize;
private:
	TStreamNode _ValtyHead;
	TStreamNode _ValtyTail;
	TStreamNode _ValtyTemp;

	friend class CollectionExtension<T>;
};

} // namespace JCore