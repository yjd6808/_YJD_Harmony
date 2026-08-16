/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Comparator.h"
#include "jc/Container/CollectionStreamIterator.h"
#include "jc/Container/Vector.h"
#include "jc/Container/LinkedList.h"

NS_JC_BEGIN

template <typename T>
struct StreamNode
{
private:
	using TStreamNode = StreamNode<T>;

public:
	T* pValue_ = nullptr;              // 다른 콜렉션의 데이터를 참조
	TStreamNode* pNext_ = nullptr;
	TStreamNode* pPrevious_ = nullptr;

	T& Ref() const
	{
		return *pValue_;
	}
};


/*=====================================================================================
                                    콜렉션 스트림
           다른 콜렉션을 참조하는 물리적 배열, 논리적인 연결리스트 기반의 자료구조이다.
=====================================================================================*/

template <typename T, typename TAllocator>
class CollectionStream
{
	using TStreamNode                 = StreamNode<T>;
	using TCollectionStream           = CollectionStream<T, TAllocator>;
	using TCollectionStreamIterator   = CollectionStreamIterator<T, TAllocator>;

public:
	template <typename TCollection>
	CollectionStream(TCollection* _pCollection)
	{
		int size = _pCollection->Size();
		this->size_ = size;

		if (size == 0)
		{
			ConnectNode(pHead_, pTail_);
			return;
		}

		// Memory::AllocateStatic<TStreamNode*>(sizeof(TStreamNode) * size);
		// 물리적 배열 생성
		pArray_ = TAllocator::template AllocateDynamic<TStreamNode*>(sizeof(TStreamNode) * size, allocatedSize_);

		// 논리적 연결리스트 구성
		ConnectNode(pHead_, &pArray_[0]);
		ConnectNode(&pArray_[size - 1], pTail_);

		auto enumerator = _pCollection->Begin();
		for (int index = 0; index < size - 1; ++index)
		{
			// 실질적인 참조 데이터의 포인터를 담아준다.
			pArray_[index].pValue_ = AddressOf(enumerator.Next());

			// 초기에는 바로 다음 인덱스에 위치하는 노드가 다음 원소이므로 연결해준다.
			ConnectNode(&pArray_[index], &pArray_[index + 1]);
		}

		// 마지막 원소의 참조 정보를 저장한다.
		pArray_[size - 1].pValue_ = AddressOf(enumerator.Next());
	}

public:
	// 복사 생성 금지
	CollectionStream(const TCollectionStream& _collectionStream) = delete;

	CollectionStream(TCollectionStream&& _collectionStream) noexcept
	{
		pArray_ = _collectionStream.pArray_;
		this->size_ = _collectionStream.size_;

		_collectionStream.pArray_ = nullptr;
		_collectionStream.size_ = 0;

		if (this->size_ == 0)
		{
			ConnectNode(pHead_, pTail_);
			return;
		}

		ConnectNode(pHead_, _collectionStream.pHead_->pNext_);
		ConnectNode(_collectionStream.pTail_->pPrevious_, pTail_);
	}

	~CollectionStream() noexcept
	{
		if (pArray_)
			TAllocator::template DeallocateDynamic(pArray_, allocatedSize_);
	}

	// TODO: 더미노드 없앨 시 수정
	TCollectionStreamIterator Begin() const
	{
		return TCollectionStreamIterator(pHead_->pNext_, pHead_, pTail_);
	}

	TCollectionStreamIterator End() const
	{
		return TCollectionStreamIterator(pTail_->pPrevious_, pHead_, pTail_);
	}

public:
	template <typename Consumer>
	TCollectionStream& ForEach(Consumer _consumer)
	{
		TStreamNode* pCurrent = pHead_->pNext_;
		while (pCurrent != pTail_)
		{
			_consumer(pCurrent->Ref());
			pCurrent = pCurrent->pNext_;
		}
		return *this;
	}

	template <typename IndexConsumer>
	TCollectionStream& ForEachWithIndex(IndexConsumer _consumer)
	{
		TStreamNode* pCurrent = pHead_->pNext_;
		int index = 0;
		while (pCurrent != pTail_)
		{
			_consumer(pCurrent->Ref(), index);
			pCurrent = pCurrent->pNext_;
			++index;
		}
		return *this;
	}

	template <typename TPredicate>
	TCollectionStream& Filter(TPredicate&& _predicate)
	{
		TStreamNode* pCurrent = pHead_->pNext_;
		int size = 0;
		while (pCurrent != pTail_)
		{
			if (_predicate(pCurrent->Ref()))
			{
				++size;
			}
			else
			{
				ConnectNode(pCurrent->pPrevious_, pCurrent->pNext_);
			}
			pCurrent = pCurrent->pNext_;
		}

		this->size_ = size;
		return *this;
	}

	TCollectionStream& Sorted()
	{
		return Sorted(NaturalOrder{});
	}

	template <typename TPredicate>
	TCollectionStream& Sorted(TPredicate&& _predicate)
	{
		MergeSort(Move(_predicate));
		return *this;
	}

	T& First() const
	{
		jc_assert_msg(this->size_ != 0, "데이터가 없습니다.");
		return *pHead_->pNext_->pValue_;
	}

	T& Last() const
	{
		jc_assert_msg(this->size_ != 0, "데이터가 없습니다.");
		return *pTail_->pPrevious_->pValue_;
	}

	Vector<T, TAllocator> ToVector()
	{
		Vector<T, TAllocator> vector;
		vector.PushBackAll(*this);
		return vector;
	}

	LinkedList<T, TAllocator> ToLinkedList()
	{
		LinkedList<T, TAllocator> list;
		list.PushBackAll(*this);
		return list;
	}

	int Size() const
	{
		return size_;
	}

	bool IsEmpty() const
	{
		return size_ == 0;
	}

protected:
	static void ConnectNode(TStreamNode* _pLeft, TStreamNode* _pRight)
	{
		_pLeft->pNext_ = _pRight;
		_pRight->pPrevious_ = _pLeft;
	}

	static TStreamNode* EndNode(TStreamNode* _pBegin)
	{
		while (_pBegin->pNext_ != nullptr)
		{
			_pBegin = _pBegin->pNext_;
		}
		return _pBegin;
	}

	template <typename TPredicate>
	void MergeSort(TPredicate&& _predicate)
	{
		// 데이터가 1개 이하인 경우는 정렬 자체를 해줄 필요가 없다.
		if (this->size_ <= 1)
		{
			return;
		}

		TStreamNode* pBegin = pHead_->pNext_;
		TStreamNode* pEnd = pTail_->pPrevious_;

		// 임시로 더미노드와 연결을 끊어준다.
		pBegin->pPrevious_ = nullptr;
		pEnd->pNext_ = nullptr;

		TStreamNode* pSorted = MergeSort(pBegin, Move(_predicate));

		if (pSorted == nullptr)
		{
			return;
		}

		// 다시 더미노드와 연결해준다.
		ConnectNode(pHead_, pSorted);
		ConnectNode(EndNode(pSorted), pTail_);
	}

	template <typename TPredicate>
	TStreamNode* MergeSort(TStreamNode* _pBegin, TPredicate&& _predicate)
	{
		if (_pBegin == nullptr)
		{
			return nullptr;
		}

		TStreamNode* pSlow = _pBegin;
		TStreamNode* pFast = _pBegin;

		while (pFast != nullptr && pFast->pNext_ != nullptr)
		{
			pSlow = pSlow->pNext_;
			pFast = pFast->pNext_->pNext_;
		}

		TStreamNode* pLeftBegin = _pBegin;
		TStreamNode* pLeftEnd = pSlow->pPrevious_;

		TStreamNode* pRightBegin = pSlow;

		if (pLeftBegin == pRightBegin)
		{
			return pLeftBegin;
		}

		if (pLeftEnd != nullptr)
		{
			pLeftEnd->pNext_ = nullptr;
		}

		if (pRightBegin != nullptr)
		{
			pRightBegin->pPrevious_ = nullptr;
		}

		pLeftBegin = MergeSort(pLeftBegin, Move(_predicate));
		pRightBegin = MergeSort(pRightBegin, Move(_predicate));

		return Merge(pLeftBegin, pRightBegin, Move(_predicate));
	}

	template <typename TPredicate>
	TStreamNode* Merge(TStreamNode* _pLeftBegin, TStreamNode* _pRightBegin, TPredicate&& _predicate)
	{
		TStreamNode* pTemp = &tempNode_;

		while (_pLeftBegin != nullptr && _pRightBegin != nullptr)
		{
			if (_predicate(_pLeftBegin->Ref(), _pRightBegin->Ref()))
			{
				ConnectNode(pTemp, _pLeftBegin);
				_pLeftBegin = _pLeftBegin->pNext_;
			}
			else
			{
				ConnectNode(pTemp, _pRightBegin);
				_pRightBegin = _pRightBegin->pNext_;
			}

			pTemp = pTemp->pNext_;
		}

		if (_pLeftBegin != nullptr)
		{
			ConnectNode(pTemp, _pLeftBegin);
		}
		else if (_pRightBegin != nullptr)
		{
			ConnectNode(pTemp, _pRightBegin);
		}

		tempNode_.pNext_->pPrevious_ = nullptr;
		return tempNode_.pNext_;
	}

protected:
	TStreamNode* pArray_ = nullptr;
	TStreamNode* pHead_ = &headNode_;
	TStreamNode* pTail_ = &tailNode_;

private:
	int allocatedSize_{};
	int size_{};

	// TODO: 추후 시간나면 더미노드 없앨 것
	TStreamNode headNode_{};
	TStreamNode tailNode_{};
	TStreamNode tempNode_{};

	friend class CollectionStreamIterator<T, TAllocator>;
};

NS_END