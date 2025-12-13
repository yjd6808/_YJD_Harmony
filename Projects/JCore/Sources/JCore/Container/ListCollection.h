/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Collection.h>
#include <JCore/Container/ListCollectionIterator.h>
#include <JCore/Container/ListNode.h>

NS_JC_BEGIN

/*=====================================================================================
								리스트 콜렉션
				 		연결리스트 기반 큐, 스택, 연결리스트 인터페이스
						  양쪽 더미 양방향 연결리스트로 구현
=====================================================================================*/

template <typename T, typename TAllocator>
class ListCollection : public Collection<T, TAllocator>
{
	using TEnumerator              = Enumerator<T, TAllocator>;
	using TListNode                = ListNode<T, TAllocator>;
	using TCollection              = Collection<T, TAllocator>;
	using TListCollection          = ListCollection<T, TAllocator>;
	using TListCollectionIterator  = ListCollectionIterator<T, TAllocator>;

public:
	ListCollection()
	{
		/* [더미노드 방법 1]
		pHead_ = Memory::AllocateStatic<TListNode*>(sizeof(TListNode) * 2); 	// 양쪽 더미를 한번에 생성하자.
		pTail_ = &pHead_[1];

		--> 더미를 굳이 동적할당할 필요가 없지않나..?
		*/

		// [더미노드 대안]
		// pHead_ = &m_ValtyHead;
		// pTail_ = &m_ValtyTail;

		// 어차피 더미노드는 Next와 Previous만 쓸 것이므로.. 굳이 TListNode의 Value의 디폴트 생성자를 호출해줄 필요가 없다.
		// Memory::PlacementAllocate(pHead_[0]);
		// Memory::PlacementAllocate(pHead_[1]);

		// [더미노드가 너무 무겁다]
		// 없는게 훨씬 메모리 관리측면에서 좋음
		// 다루는 데이터가 무거워지니까 메모리뻥튀기가 너무심하다.
		pHead_ = nullptr;
		pTail_ = nullptr;
		size_ = 0;
	}

	ListCollection(const TListCollection& _otherCollection)
		: ListCollection()
	{
		CopyFrom(_otherCollection);
	}

	ListCollection(TListCollection&& _otherCollection) noexcept
		: ListCollection()
	{
		CopyFrom(Move(_otherCollection));
	}

	ListCollection(std::initializer_list<T> _ilist)
		: ListCollection()
	{
		CopyFrom(_ilist);
	}

	~ListCollection() noexcept override = 0;

public:
	virtual void Clear()
	{
		/*
		 
		     ■ <- ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
		     ↑                                        ↑     ↑
		    Head                                    pCur  Tail
			      <----------------------------------->
				           이 영역만 삭제해준다.


			그리고 남은 더미 노드 2개(머리, 꼬리)를 연결해준다.
		 */

		TListNode* pCur = pHead_;

		while (pCur != nullptr)
		{
			TListNode* pTemp = pCur;
			pCur = pCur->pNext_;
			pTemp->DeleteSelf();
		}

		pHead_ = nullptr;
		pTail_ = nullptr;

		size_ = 0;
	}

	template <typename Consumer>
	void ForEach(Consumer&& _consumer)
	{
		TListNode* pCur = pHead_;
		while (pCur != nullptr)
		{
			TListNode* pNext = pCur->pNext_;
			_consumer(pCur->value_);
			pCur = pNext;
		}
	}

	void ForEachDelete()
	{
		if constexpr (!IsPointerType_v<T>)
		{
			DebugAssert(false);
			return;
		}

		TListNode* pCur = pHead_;
		while (pCur != nullptr)
		{
			TListNode* pNext = pCur->pNext_;
			delete pCur->value_;
			pCur = pNext;
		}
	}

	bool IsEmpty() const override { return size_ == 0; }
	int Size() const override { return size_; }

protected:
	template <typename U = T, typename UAllocator>
	void CopyFrom(const ListCollection<U, UAllocator>& _otherCollection)
	{
		static_assert(IsSameType_v<U, T>, "... U and T is difference type.");
		static_assert(IsAssignable_v<U&, const T&>, "... U cannot be assign to T (T = U is impossible operation)");

		using UListNode = ListNode<U, UAllocator>;
		DebugAssertMsg(this != &_otherCollection, "자기 자신에게 대입할 수 없습니다.");

		TListNode* pPrev = nullptr;
		TListNode* pCur = pHead_;
		UListNode* pOtherCur = _otherCollection.pHead_;

		// 기존에 이미 메모리 할당된 녀석은 데이터만 복사해준다.
		while (pCur != nullptr && pOtherCur != nullptr)
		{
			Memory::PlacementDelete(pCur->value_);
			pCur->value_ = pOtherCur->value_;
			pPrev = pCur;
			pCur = pCur->pNext_;
			pOtherCur = pOtherCur->pNext_;
		}

		// [Case 1] 만약 기존에 할당된 데이터가 더 많다면 
		//             = pOtherCur이 other.pTail_이 충족되어 먼저 반복문을 빠져나온 경우
		// 복사된 마지막 노드 다음 노드부터 삭제를 해준다.
		//    = pCur->pPrevious_가 복사된 마지막 노드이고
		//    = pCur : 삭제되어야할 노드들이다.
		if (size_ > _otherCollection.size_)
		{
			RemoveNodesEnd(pCur);
			pTail_ = pPrev;
			pTail_->pNext_ = nullptr;
		}

		// [Case 2] 만약 복사할 데이터가 더 많다면
		while (pOtherCur != nullptr)
		{
			PushBack(pOtherCur->value_);
			pOtherCur = pOtherCur->pNext_;
		}

		size_ = _otherCollection.size_;
	}

	virtual void CopyFrom(TListCollection&& _otherCollection)
	{
		// this->ThrowIfAssignSelf(other);
		Clear();

		this->owner_ = Move(_otherCollection.owner_);

		// 만약 비어있을 경우 other의 더미 헤드와 더미 테일을 참조하게 되는데
		// 동적할당된 녀석이 아니기 때문에 나중에 오류를 일으키게 된다.
		// 그래서 데이터가 없는 경우는 그냥 나가면 됨
		if (_otherCollection.size_ == 0)
		{
			return;
		}

		pTail_ = _otherCollection.pTail_;
		pHead_ = _otherCollection.pHead_;

		size_ = _otherCollection.size_;

		_otherCollection.pHead_ = nullptr;
		_otherCollection.pTail_ = nullptr;
		_otherCollection.size_ = 0;
	}

	template <typename U = T>
	void CopyFrom(std::initializer_list<U> _ilist)
	{
		static_assert(IsSameType_v<U, T>, "... U and T is difference type.");
		static_assert(IsAssignable_v<U&, const T&>, "... U cannot be assign to T (T = U is impossible operation)");

		TListNode* pPrev = nullptr;
		TListNode* pCur = pHead_;
		auto pOtherCur = _ilist.begin();

		// 기존에 이미 메모리 할당된 녀석은 데이터만 복사해준다.
		while (pCur != nullptr && pOtherCur != _ilist.end())
		{
			Memory::PlacementDelete(pCur->value_);
			pCur->value_ = *pOtherCur;
			pPrev = pCur;
			pCur = pCur->pNext_;
			++pOtherCur;
		}

		// [Case 1] 만약 기존에 할당된 데이터가 더 많다면 
		if (size_ > static_cast<int>(_ilist.size()))
		{
			RemoveNodesEnd(pCur);
			pTail_ = pPrev;
			pTail_->pNext_ = nullptr;
		}

		// [Case 2] 만약 복사할 데이터가 더 많다면
		while (pOtherCur != _ilist.end())
		{
			PushBack(*pOtherCur);
			++pOtherCur;
		}

		size_ = static_cast<int>(_ilist.size());
	}

	virtual bool Valid() const
	{
		return true;
	}

	/// <summary>
	/// exclusiveFirst와 exclusiveLast 노드 사이에 존재하는 노드들을 삭제한다.
	/// </summary>
	void RemoveNodesBetween(TListNode* _pExclusiveFirst, TListNode* _pExclusiveLast)
	{
		TListNode* pDel = _pExclusiveFirst->pNext_;

		while (pDel != _pExclusiveLast)
		{
			TListNode* pTemp = pDel;
			pDel = pDel->pNext_;
			pTemp->DeleteSelf();
		}

		ConnectNode(_pExclusiveFirst, _pExclusiveLast);
	}

	/**
	 * \brief exclusiveFirst부터 끝까지 모두 제거한다.
	 */
	void RemoveNodesEnd(TListNode* _pInclusiveFirst)
	{
		TListNode* pDel = _pInclusiveFirst;

		while (pDel != nullptr)
		{
			TListNode* pTemp = pDel;
			pDel = pDel->pNext_;
			pTemp->DeleteSelf();
		}
	}

	virtual void PushBack(const T& _data)
	{
		TListNode* pNewNode = CreateNewNode(_data);
		PushBackNewNode(pNewNode);
		++size_;
	}

	virtual void PushBack(T&& _data)
	{
		TListNode* pNewNode = CreateNewNode(Move(_data));
		PushBackNewNode(pNewNode);
		++size_;
	}

	virtual void PushBackAll(const TCollection& _collection)
	{
		TEnumerator it = _collection.Begin();
		while (it->HasNext())
		{
			TListNode* pNewNode = CreateNewNode(it->Next());
			PushBackNewNode(pNewNode);
		}
		size_ += _collection.Size();
	}

	/// <summary>
	///
	/// </summary>
	/// <typeparam name="U">
	///		템플릿 함수 인스턴스화를 방지하기위해 템플릿 함수로 만들었다.
	///		
	///		FindNode 함수에서 T 타입에 대해서 동등비교를 실행하는데 ListCollection의 자식 컨테이너가 템플릿 인스턴스화가 되면
	///		비교 연산자가 없는 구조체나 클래스들에 대해서 오류를 발생시킨다.
	///
	///		그래서 Remove, FindNode 함수를 사용할 경우에만 동등 비교가 가능여부에 대해서 오류를 뛰우도록 하였다.
	/// </typeparam>
	template <typename U = T>
	bool Remove(const U& _data)
	{
		TListNode* pDel = FindNode(_data);

		if (pDel == nullptr)
		{
			return false;
		}

		RemoveNode(pDel);
		pDel->DeleteSelf();
		--size_;
		return true;
	}

	virtual bool Remove(TListCollectionIterator& _iter)
	{
		if (_iter.pHead_ != pHead_)
		{
			throw InvalidOperationException("해당 이터레이터가 소속된 컨테이너를 제대로 지정해주세요.");
		}

		if (_iter.pCurrent_ == pHead_ || _iter.pCurrent_ == pTail_)
		{
			throw InvalidOperationException("이터레이터가 처음 또는 끝을 가리키고 있습니다.");
		}

		TListNode* pDel = _iter.pCurrent_;
		_iter.pCurrent_ = pDel->pNext_;
		RemoveNode(pDel);
		pDel->DeleteSelf();
		--size_;
		return true;
	}

	void RemoveNode(TListNode* _pDelNode)
	{
		if (_pDelNode == pHead_)
		{
			pHead_ = pHead_->pNext_;
			if (pHead_ == nullptr)
			{
				pTail_ = nullptr;
			}
			else
			{
				pHead_->pPrevious_ = nullptr;
			}
		}
		else if (_pDelNode == pTail_)
		{
			pTail_ = pTail_->pPrevious_;
			if (pTail_ == nullptr)
			{
				pHead_ = nullptr;
			}
			else
			{
				pTail_->pNext_ = nullptr;
			}
		}
		else
		{
			this->ConnectNode(_pDelNode->pPrevious_, _pDelNode->pNext_);
		}
	}

	/// <summary>
	/// node 바로 전에 newNode를 삽입한다.
	/// </summary>
	void PushBackNewNode(TListNode* _pNewNode)
	{
		if (pHead_ == nullptr)
		{
			pHead_ = _pNewNode;
			pTail_ = _pNewNode;
			return;
		}

		/*
			[삽입 전]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
							         ↑     ↑
								          node
							      node->prev

			[삽입 후]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
							         ↑     ↑     ↑
							     node->prev     node
								     newNode
		 */

		ConnectNode(pTail_, _pNewNode);

		pTail_ = _pNewNode;
	}

	virtual void PushFront(const T& _data)
	{
		TListNode* pNewNode = CreateNewNode(_data);
		DebugAssert(pNewNode->pPrevious_ == nullptr);
		PushFrontNewNode(pNewNode);
		++size_;
	}

	virtual void PushFront(T&& _data)
	{
		TListNode* pNewNode = CreateNewNode(Move(_data));
		DebugAssert(pNewNode->pPrevious_ == nullptr);
		PushFrontNewNode(pNewNode);
		++size_;
	}

	virtual void PushFrontAll(const TCollection& _collection)
	{
		TEnumerator it = _collection.Begin();
		while (it->HasNext())
		{
			TListNode* pNewNode = CreateNewNode(it->Next());
			PushFrontNewNode(pNewNode);
		}
		size_ += _collection.Size();
	}

	/// <summary>
	/// node 바로 이후에 newNode를 삽입한다.
	/// </summary>
	void PushFrontNewNode(TListNode* _pNewNode)
	{
		if (pHead_ == nullptr)
		{
			pHead_ = _pNewNode;
			pTail_ = _pNewNode;
			return;
		}

		/*
			[삽입 전]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■

			[삽입 후]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
			 ↑ 
		   newNode
		 */

		ConnectNode(_pNewNode, pHead_);
		pHead_ = _pNewNode;
		DebugAssert(pHead_->pPrevious_ == nullptr);
	}

	template <typename... Args>
	void EmplaceBack(Args&&... _args)
	{
		TListNode* pNewNode = EmplaceNewNode(Forward<Args>(_args)...);
		PushBackNewNode(pNewNode);
		++size_;
	}

	template <typename... Args>
	void EmplaceFront(Args&&... _args)
	{
		TListNode* pNewNode = EmplaceNewNode(Forward<Args>(_args)...);
		PushFrontNewNode(pNewNode);
		++size_;
	}

	virtual void PopFront()
	{
		DebugAssertMsg(size_ != 0, "데이터가 없습니다.");

		TListNode* pDel = pHead_;
		pHead_ = pHead_->pNext_;
		if (pHead_ == nullptr)
		{
			pTail_ = nullptr;
		}
		else
		{
			pHead_->pPrevious_ = nullptr;
		}
		pDel->DeleteSelf();
		--size_;
	}

	virtual void PopBack()
	{
		DebugAssertMsg(size_ != 0, "데이터가 없습니다.");

		TListNode* pDel = pTail_;
		pTail_ = pTail_->pPrevious_;
		if (pTail_ == nullptr)
		{
			pHead_ = nullptr;
		}
		else
		{
			pTail_->pNext_ = nullptr;
		}
		pDel->DeleteSelf();
		--size_;
	}

	virtual T& Front() const
	{
		DebugAssertMsg(size_ != 0, "데이터가 없습니다.");
		return pHead_->value_;
	}

	virtual T& Back() const
	{
		DebugAssertMsg(size_ != 0, "데이터가 없습니다.");
		return pTail_->value_;
	}

	TListNode* CreateNewNode(const T& _data)
	{
		if constexpr (!IsCopyConstructible_v<T>)
		{
			DebugAssertMsg(false, "복사 생성할 수 없는 객체입니다.");
			return nullptr;
		}
		else
		{
			TListNode* pNewNode = TAllocator::template AllocateInitStatic<TListNode>();
			pNewNode->Construct(_data);
			return pNewNode;
		}
	}

	TListNode* CreateNewNode(T&& _data)
	{
		if constexpr (!IsMoveConstructible_v<T>)
		{
			DebugAssertMsg(false, "이동 생성할 수 없는 객체입니다.");
			return nullptr;
		}
		else
		{
			TListNode* pNewNode = TAllocator::template AllocateInitStatic<TListNode>();
			pNewNode->Construct(Move(_data));
			return pNewNode;
		}
	}

	template <typename... Args>
	TListNode* EmplaceNewNode(Args&&... _args)
	{
		TListNode* pNewNode = TAllocator::template AllocateInitStatic<TListNode>();
		pNewNode->Construct(Forward<Args>(_args)...);
		return pNewNode;
	}

	/// <summary>
	/// 노드끼리 서로 연결한다.
	///
	/// lhs의 다음 노드는 rhs로 설정하고
	/// rhs의 이전 노드는 lhs로 설정한다.
	/// </summary>
	static void ConnectNode(TListNode* _pLhs, TListNode* _pRhs)
	{
		if (_pLhs != nullptr)
		{
			_pLhs->pNext_ = _pRhs;
		}

		if (_pRhs != nullptr)
		{
			_pRhs->pPrevious_ = _pLhs;
		}
	}

	template <typename U = T>
	TListNode* FindNode(const U& _data) const
	{
		TListNode* pCur = pHead_;
		while (pCur != nullptr)
		{
			if (pCur->value_ == _data)
			{
				return pCur;
			}
			pCur = pCur->pNext_;
		}

		return nullptr;
	}

	template <typename TPredicate>
	TListNode* FindNodeIf(TPredicate&& _predicate)
	{
		TListNode* pCur = pHead_;
		while (pCur != nullptr)
		{
			if (_predicate(pCur->value_))
			{
				return pCur;
			}
			pCur = pCur->pNext_;
		}

		return nullptr;
	}

	CollectionType GetCollectionType() override { return CollectionType::List; }

protected:
	TListNode* pHead_;
	TListNode* pTail_;
	int size_;

	friend class TListCollectionIterator;
};


template <typename T, typename TAllocator>
ListCollection<T, TAllocator>::~ListCollection() noexcept
{
	Clear();

	// 더미노드 제거
	// Memory::DeallocateStatic(pHead_);
}


NS_JC_END
