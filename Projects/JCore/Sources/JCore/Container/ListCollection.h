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
	using TEnumerator				= Enumerator<T, TAllocator>;
	using TListNode					= ListNode<T, TAllocator>;
	using TCollection				= Collection<T, TAllocator>;
	using TListCollection			= ListCollection<T, TAllocator>;
	using TListCollectionIterator   = ListCollectionIterator<T, TAllocator>;
public:
	ListCollection() {
		/* [더미노드 방법 1]
		m_pHead = Memory::Allocate<TListNode*>(sizeof(TListNode) * 2);	// 양쪽 더미를 한번에 생성하자.
		m_pTail = &m_pHead[1];

		--> 더미를 굳이 동적할당할 필요가 없지않나..?
		*/

		// [더미노드 대안]
		// m_pHead = &m_ValtyHead;
		// m_pTail = &m_ValtyTail;

		// 어차피 더미노드는 Next와 Previous만 쓸 것이므로.. 굳이 TListNode의 Value의 디폴트 생성자를 호출해줄 필요가 없다.
		// Memory::PlacementAllocate(m_pHead[0]);
		// Memory::PlacementAllocate(m_pHead[1]);

		// [더미노드가 너무 무겁다]
		// 없는게 훨씬 메모리 관리측면에서 좋음
		// 다루는 데이터가 무거워지니까 메모리뻥튀기가 너무심하다.
		m_pHead = nullptr;
		m_pTail = nullptr;
	}

	ListCollection(const TListCollection& other) : TListCollection() {
		CopyFrom(other);
	}

	ListCollection(TListCollection&& other) noexcept : TListCollection() {
		CopyFrom(Move(other));
	}

	ListCollection(std::initializer_list<T> ilist) : TListCollection() {
		CopyFrom(ilist);
	}

	~ListCollection() noexcept override = 0;
public:
	virtual void Clear() {
		/*
		 
		     ■ <- ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
		     ↑                                        ↑     ↑
		    Head                                    pCur  Tail
			      <----------------------------------->
				           이 영역만 삭제해준다.


			그리고 남은 더미 노드 2개(머리, 꼬리)를 연결해준다.
		 */

		TListNode* pCur = m_pHead;

		while (pCur != nullptr) {
			TListNode* pTemp = pCur;
			pCur = pCur->Next;
			pTemp->DeleteSelf();
		}

		m_pHead = nullptr;
		m_pTail = nullptr;

		this->m_iSize = 0;
	}
protected:

	template <typename U = T, typename UAllocator>
	void CopyFrom(const ListCollection<U, UAllocator>& other) {
		static_assert(IsSameType_v<U, T>, "... U and T is difference type.");
		static_assert(IsAssignable_v<U&, const T&>, "... U cannot be assign to T (T = U is impossible operation)");

		using UListNode = ListNode<U, UAllocator>;
		DebugAssertMsg(this != &other, "자기 자신에게 대입할 수 없습니다.");

		TListNode* pPrev = nullptr;
		TListNode* pCur = m_pHead;
		UListNode* pOtherCur = other.m_pHead;

		// 기존에 이미 메모리 할당된 녀석은 데이터만 복사해준다.
		while (pCur != nullptr && pOtherCur != nullptr) {
			Memory::PlacementDelete(pCur->Value);
			pCur->Value = pOtherCur->Value;
			pPrev = pCur;
			pCur = pCur->Next;
			pOtherCur = pOtherCur->Next;
		}

		

		// [Case 1] 만약 기존에 할당된 데이터가 더 많다면 
		//             = pOtherCur이 other.m_pTail이 충족되어 먼저 반복문을 빠져나온 경우
		// 복사된 마지막 노드 다음 노드부터 삭제를 해준다.
		//    = pCur->Previous가 복사된 마지막 노드이고
		//    = pCur : 삭제되어야할 노드들이다.
		if (this->m_iSize > other.m_iSize) {
			RemoveNodesEnd(pCur);
			m_pTail = pPrev;
			m_pTail->Next = nullptr;
		}
		

		// [Case 2] 만약 복사할 데이터가 더 많다면
		while (pOtherCur != nullptr) {
			PushBack(pOtherCur->Value);
			pOtherCur = pOtherCur->Next;
		}

		this->m_iSize = other.m_iSize;
	}

	virtual void CopyFrom(TListCollection&& other) {
		// this->ThrowIfAssignSelf(other);
		Clear();

		this->m_Owner = Move(other.m_Owner);

		// 만약 비어있을 경우 other의 더미 헤드와 더미 테일을 참조하게 되는데
		// 동적할당된 녀석이 아니기 때문에 나중에 오류를 일으키게 된다.
		// 그래서 데이터가 없는 경우는 그냥 나가면 됨
		if (other.m_iSize == 0) {
			return;
		}
		

		m_pTail = other.m_pTail;
		m_pHead = other.m_pHead;

		this->m_iSize = other.m_iSize;

		other.m_pHead = nullptr;
		other.m_pTail = nullptr;
		other.m_iSize = 0;
	}

	template <typename U = T>
	void CopyFrom(std::initializer_list<U> ilist) {
		static_assert(IsSameType_v<U, T>, "... U and T is difference type.");
		static_assert(IsAssignable_v<U&, const T&>, "... U cannot be assign to T (T = U is impossible operation)");

		TListNode* pPrev = nullptr;
		TListNode* pCur = m_pHead;
		auto pOtherCur = ilist.begin();

		// 기존에 이미 메모리 할당된 녀석은 데이터만 복사해준다.
		while (pCur != nullptr && pOtherCur != ilist.end()) {
			Memory::PlacementDelete(pCur->Value);
			pCur->Value = *pOtherCur;
			pPrev = pCur;
			pCur = pCur->Next;
			++pOtherCur;
		}

		// [Case 1] 만약 기존에 할당된 데이터가 더 많다면 
		if (this->m_iSize > ilist.size()) {
			RemoveNodesEnd(pCur);
			m_pTail = pPrev;
			m_pTail->Next = nullptr;
		}

		// [Case 2] 만약 복사할 데이터가 더 많다면
		while (pOtherCur != ilist.end()) {
			PushBack(*pOtherCur);
			++pOtherCur;
		}

		this->m_iSize = ilist.size();
	}

	virtual bool Valid() const {
		return true;
	}

	/// <summary>
	/// exclusiveFirst와 exclusiveLast 노드 사이에 존재하는 노드들을 삭제한다.
	/// </summary>
	void RemoveNodesBetween(TListNode* exclusiveFirst, TListNode* exclusiveLast) {
		TListNode* pDel = exclusiveFirst->Next;

		while (pDel != exclusiveLast) {
			TListNode* pTemp = pDel;
			pDel = pDel->Next;
			pTemp->DeleteSelf();
		}

		ConnectNode(exclusiveFirst, exclusiveLast);
	}

	/**
	 * \brief exclusiveFirst부터 끝까지 모두 제거한다.
	 */
	void RemoveNodesEnd(TListNode* inclusiveFirst) {
		TListNode* pDel = inclusiveFirst;

		while (pDel != nullptr) {
			TListNode* pTemp = pDel;
			pDel = pDel->Next;
			pTemp->DeleteSelf();
		}
	}

	virtual void PushBack(const T& data) {
        TListNode* pNewNode = CreateNewNode(data);
        PushBackNewNode(pNewNode);
        ++this->m_iSize;
	}


	virtual void PushBack(T&& data) {
		TListNode* pNewNode = CreateNewNode(Move(data));
		PushBackNewNode(pNewNode);
		++this->m_iSize;
	}

	virtual void PushBackAll(const TCollection& collection) {
		TEnumerator it = collection.Begin();
		while (it->HasNext()) {
			TListNode* pNewNode = CreateNewNode(it->Next());
			PushBackNewNode(pNewNode);
		}
		this->m_iSize += collection.Size();
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
	bool Remove(const U& data) {
		TListNode* pDel = FindNode(data);

		if (pDel == nullptr) {
			return false;
		}

		RemoveNode(pDel);
		pDel->DeleteSelf();
		--this->m_iSize;
		return true;
	}

	virtual bool Remove(TListCollectionIterator& iter) {
		if (iter.m_pHead != this->m_pHead) {
			throw InvalidOperationException("해당 이터레이터가 소속된 컨테이너를 제대로 지정해주세요.");
		}

		if (iter.m_pCurrent == this->m_pHead || iter.m_pCurrent == this->m_pTail) {
			throw InvalidOperationException("이터레이터가 처음 또는 끝을 가리키고 있습니다.");
		}

		TListNode* pDel = iter.m_pCurrent;
		iter.m_pCurrent = pDel->Next;
		RemoveNode(pDel);
		pDel->DeleteSelf();
		--this->m_iSize;
		return true;
	}

	void RemoveNode(TListNode* delNode) {
		if (delNode == m_pHead) {
			m_pHead = m_pHead->Next;
			if (m_pHead == nullptr) m_pTail = nullptr;
			else m_pHead->Previous = nullptr;
		} else if (delNode == m_pTail) {
			m_pTail = m_pTail->Previous;
			if (m_pTail == nullptr) m_pHead = nullptr;
			else m_pTail->Next = nullptr;
		} else {
			this->ConnectNode(delNode->Previous, delNode->Next);
		}
	}

	/// <summary>
	/// node 바로 전에 newNode를 삽입한다.
	/// </summary>
	void PushBackNewNode(TListNode* newNode) {
		if (m_pHead == nullptr) {
			m_pHead = newNode;
			m_pTail = newNode;
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
		
		ConnectNode(m_pTail, newNode);

		m_pTail = newNode;
	}

	virtual void PushFront(const T& data) {
		TListNode* pNewNode = CreateNewNode(data);
		PushFrontNewNode(pNewNode);
		++this->m_iSize;
	}

	virtual void PushFront(T&& data) {
		TListNode* pNewNode = CreateNewNode(Move(data));
		PushFrontNewNode(pNewNode);
		++this->m_iSize;
	}

	virtual void PushFrontAll(const TCollection& collection) {
		TEnumerator it = collection.Begin();
		while (it->HasNext()) {
			TListNode* pNewNode = CreateNewNode(it->Next());
			PushFrontNewNode(pNewNode);
		}
		this->m_iSize += collection.Size();
	}

	/// <summary>
	/// node 바로 이후에 newNode를 삽입한다.
	/// </summary>
	void PushFrontNewNode(TListNode* newNode) {
		if (m_pHead == nullptr) {
			m_pHead = newNode;
			m_pTail = newNode;
		}

		/*
			[삽입 전]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■

			[삽입 후]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
			 ↑ 
		   newNode
		 */

		ConnectNode(newNode, m_pHead);
		m_pHead = newNode;
	}

	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		TListNode* pNewNode = EmplaceNewNode(Forward<Args>(args)...);
		PushBackNewNode(pNewNode);
		++this->m_iSize;
	}

	template <typename... Args>
	void EmplaceFront(Args&&... args) {
		TListNode* pNewNode = EmplaceNewNode(Forward<Args>(args)...);
		PushFrontNewNode(pNewNode);
		++this->m_iSize;
	}


	virtual void PopFront() {
		DebugAssertMsg(this->m_iSize != 0, "데이터가 없습니다.");
		
		TListNode* pDel = m_pHead;
		m_pHead = m_pHead->Next;
		if (m_pHead == nullptr) m_pTail = nullptr;
		else m_pHead->Previous = nullptr;
		pDel->DeleteSelf();
		--this->m_iSize;
	}

	virtual void PopBack() {
		DebugAssertMsg(this->m_iSize != 0, "데이터가 없습니다.");

		TListNode* pDel = m_pTail;
		m_pTail = m_pTail->Previous;
		if (m_pTail == nullptr) m_pHead = nullptr;
		else m_pTail->Next = nullptr;

		pDel->DeleteSelf();
		--this->m_iSize;
	}

	virtual T& Front() const {
		DebugAssertMsg(this->m_iSize != 0, "데이터가 없습니다.");
		return m_pHead->Value;
	}

	virtual T& Back() const {
		DebugAssertMsg(this->m_iSize != 0, "데이터가 없습니다.");
		return m_pTail->Value;
	}

	TListNode* CreateNewNode(const T& data) {
		if constexpr (!IsCopyConstructible_v<T>) {
            DebugAssertMsg(false, "복사 생성할 수 없는 객체입니다.");
            return nullptr;
		} else {
			TListNode* pNewNode = TAllocator::template AllocateInit<TListNode>();
            pNewNode->Construct(data);
            return pNewNode;
		}
	}

	TListNode* CreateNewNode(T&& data) {
        if constexpr (!IsMoveConstructible_v<T>) {
            DebugAssertMsg(false, "이동 생성할 수 없는 객체입니다.");
            return nullptr;
        } else {
			TListNode* pNewNode = TAllocator::template AllocateInit<TListNode>();
            pNewNode->Construct(Move(data));
            return pNewNode;
        }
	}

	template <typename... Args>
	TListNode* EmplaceNewNode(Args&&... args) {
		TListNode* pNewNode = TAllocator::template AllocateInit<TListNode>();
		pNewNode->Construct(Forward<Args>(args)...);
		return pNewNode;
	}

	/// <summary>
	/// 노드끼리 서로 연결한다.
	/// 
	/// lhs의 다음 노드는 rhs로 설정하고
	/// rhs의 이전 노드는 lhs로 설정한다.
	/// </summary>
	static void ConnectNode(TListNode* lhs, TListNode* rhs) {
		if (lhs != nullptr)
			lhs->Next = rhs;

		if (rhs != nullptr)
			rhs->Previous = lhs;
	}

	template <typename U = T>
	TListNode* FindNode(const U& data) const {
		TListNode* pCur = m_pHead;
		while (pCur != nullptr) {
			if (pCur->Value == data) {
				return pCur;
			}
			pCur = pCur->Next;
		}

		return nullptr;
	}

	template <typename TPredicate>
	TListNode* FindNodeIf(TPredicate&& predicate) {
		TListNode* pCur = m_pHead;
		while (pCur != nullptr) {
			if (predicate(pCur->Value)) {
				return pCur;
			}
			pCur = pCur->Next;
		}

		return nullptr;
	}

	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		TListNode* pCur = m_pHead;
		while (pCur != nullptr) {
			consumer(pCur->value);
			pCur = pCur->Next;
		}
	}

	CollectionType GetCollectionType() override { return CollectionType::List; }
protected:
	TListNode* m_pHead;
	TListNode* m_pTail;

	friend class TListCollectionIterator;
};


template <typename T, typename TAllocator>
ListCollection<T, TAllocator>::~ListCollection() noexcept {
	Clear();

	// 더미노드 제거
	// Memory::Deallocate(m_pHead);
}


NS_JC_END