/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/Collection.h>
#include <JCore/Container/ListCollectionIterator.h>
#include <JCore/Container/ListNode.h>

namespace JCore {

/*=====================================================================================
									리스트 콜렉션
					     연결리스트 기반 큐, 스택, 연결리스트 인터페이스
						      양쪽 더미 양방향 연결리스트로 구현
=====================================================================================*/

template <typename T>
class ListCollection : public Collection<T>
{
	using TEnumerator				= typename Enumerator<T>;
	using TListNode					= typename ListNode<T>;
	using TCollection				= typename Collection<T>;
	using TListCollection			= typename ListCollection<T>;
	using TListCollectionIterator   = typename ListCollectionIterator<T>;
public:
	ListCollection(ContainerType containerType) : TCollection(CollectionType::List, containerType)  {
		/* [더미노드 방법 1]
		m_pHead = Memory::Allocate<TListNode*>(sizeof(TListNode) * 2);	// 양쪽 더미를 한번에 생성하자.
		m_pTail = &m_pHead[1];

		--> 더미를 굳이 동적할당할 필요가 없지않나..?
		*/

		// [더미노드 대안]
		m_pHead = &_ValtyHead;
		m_pTail = &_ValtyTail;

		// 어차피 더미노드는 Next와 Previous만 쓸 것이므로.. 굳이 TListNode의 Value의 디폴트 생성자를 호출해줄 필요가 없다.
		// Memory::PlacementAllocate(m_pHead[0]);
		// Memory::PlacementAllocate(m_pHead[1]);

		m_pHead->Previous = nullptr;
		m_pTail->Next = nullptr;

		ConnectNode(m_pHead, m_pTail);
	}

	ListCollection(const TListCollection& other, ContainerType containerType) 
		: TListCollection(containerType)
	{
		CopyFrom(other);
	}

	ListCollection(TListCollection&& other, ContainerType containerType) 
		: TListCollection(containerType)
	{
		CopyFrom(Move(other));
	}

	ListCollection(std::initializer_list<T> ilist, ContainerType containerType)
		: TListCollection(containerType)
	{
		CopyFrom(ilist);
	}
	virtual ~ListCollection() noexcept = 0;
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

		TListNode* pCur = m_pTail->Previous;

		while (pCur != m_pHead) {
			TListNode* pTemp = pCur;
			pCur = pCur->Previous;
			delete pTemp;
		}

		ConnectNode(m_pHead, m_pTail);
		this->m_iSize = 0;
	}
protected:
	virtual void CopyFrom(const ListCollection& other) {
		this->ThrowIfAssignSelf(other);

		TListNode* pCur = m_pHead->Next;
		TListNode* pOtherCur = other.m_pHead->Next;

		// 기존에 이미 메모리 할당된 녀석은 데이터만 복사해준다.
		while (pCur != m_pTail && pOtherCur != other.m_pTail) {
			Memory::PlacementDeallocate(pCur->Value);
			pCur->Value = pOtherCur->Value;
			pCur = pCur->Next;
			pOtherCur = pOtherCur->Next;
		}

		// [Case 1] 만약 기존에 할당된 데이터가 더 많다면 
		//             = pOtherCur이 other.m_pTail이 충족되어 먼저 반복문을 빠져나온 경우
		// 복사된 마지막 노드 다음 노드부터 삭제를 해준다.
		//    = pCur->Previous가 복사된 마지막 노드이고
		//    = pCur : 삭제되어야할 노드들이다.
		if (this->m_iSize > other.m_iSize) {
			RemoveNodesBetween(pCur->Previous, m_pTail);
		}

		// [Case 2] 만약 복사할 데이터가 더 많다면
		while (pOtherCur != other.m_pTail) {
			PushBack(pOtherCur->Value);
			pOtherCur = pOtherCur->Next;
		}

		this->m_iSize = other.m_iSize;
	}

	virtual void CopyFrom(ListCollection&& other) {
		this->ThrowIfAssignSelf(other);
		Clear();

		// 만약 비어있을 경우 other의 더미 헤드와 더미 테일을 참조하게 되는데
		// 동적할당된 녀석이 아니기 때문에 나중에 오류를 일으키게 된다.
		// 그래서 데이터가 없는 경우는 그냥 나가면 됨
		if (other.m_iSize == 0) {
			return;
		}

		m_pTail->Previous = other.m_pTail->Previous;
		m_pHead->Next = other.m_pHead->Next;

		m_pTail->Previous->Next = m_pTail;
		m_pHead->Next->Previous = m_pHead;

		this->m_iSize = other.m_iSize;

		ConnectNode(other.m_pHead, other.m_pTail);
		other.m_iSize = 0;
	}

	virtual void CopyFrom(std::initializer_list<T> ilist) {
		TListNode* pCur = m_pHead->Next;
		auto pOtherCur = ilist.begin();

		// 기존에 이미 메모리 할당된 녀석은 데이터만 복사해준다.
		while (pCur != m_pTail && pOtherCur != ilist.end()) {
			Memory::PlacementDeallocate(pCur->Value);
			pCur->Value = *pOtherCur;
			pCur = pCur->Next;
			pOtherCur++;
		}

		// [Case 1] 만약 기존에 할당된 데이터가 더 많다면 
		if (this->m_iSize > ilist.size()) {
			RemoveNodesBetween(pCur->Previous, m_pTail);
		}

		// [Case 2] 만약 복사할 데이터가 더 많다면
		while (pOtherCur != ilist.end()) {
			PushBack(*pOtherCur);
			pOtherCur++;
		}

		this->m_iSize = ilist.size();
	}

	/// <summary>
	/// exclusiveFirst와 exclusiveLast 노드 사이에 존재하는 노드들을 삭제한다.
	/// </summary>
	void RemoveNodesBetween(TListNode* exclusiveFirst, TListNode* exclusiveLast) {
		TListNode* pDel = exclusiveFirst->Next;

		while (pDel != m_pTail) {
			TListNode* pTemp = pDel;
			pDel = pDel->Next;
			delete pTemp;
		}

		ConnectNode(exclusiveFirst, exclusiveLast);
	}

	virtual void PushBack(const T& data) {
		TListNode* pNewNode = CreateNewNode(data);
		InsertNodePrev(m_pTail, pNewNode);
		this->m_iSize++;
	}

	virtual void PushBack(T&& data) {
		TListNode* pNewNode = CreateNewNode(Move(data));
		InsertNodePrev(m_pTail, pNewNode);
		this->m_iSize++;
	}

	virtual void PushBackAll(const TCollection& collection) {
		TEnumerator it = collection.Begin();
		while (it->HasNext()) {
			TListNode* pNewNode = CreateNewNode(it->Next());
			InsertNodePrev(m_pTail, pNewNode);
		}
		this->m_iSize += collection.Size();
	}

	/// <summary>
	/// node 바로 전에 newNode를 삽입한다.
	/// </summary>
	/// <param name="tailPrev"></param>
	/// <param name="newNode"></param>
	void InsertNodePrev(TListNode* node, TListNode*  newNode) {
		if (node == m_pHead) {
			throw InvalidArgumentException("헤드 이전에는 노드를 삽입하면 안되요!");
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

		TListNode* pNodePrev = node->Previous;

		ConnectNode(pNodePrev, newNode);
		ConnectNode(newNode, node);
	}

	virtual void PushFront(const T& data) {
		TListNode* pNewNode = CreateNewNode(data);
		InsertNodeNext(m_pHead, pNewNode);
		this->m_iSize++;
	}

	virtual void PushFront(T&& data) {
		TListNode* pNewNode = CreateNewNode(Move(data));
		InsertNodeNext(m_pHead, pNewNode);
		this->m_iSize++;
	}

	virtual void PushFrontAll(const TCollection& collection) {
		TEnumerator it = collection.Begin();
		while (it->HasNext()) {
			TListNode* pNewNode = CreateNewNode(it->Next());
			InsertNodeNext(m_pHead, pNewNode);
		}
		this->m_iSize += collection.Size();
	}

	/// <summary>
	/// node 바로 이후에 newNode를 삽입한다.
	/// </summary>
	/// <param name="tailPrev"></param>
	/// <param name="newNode"></param>
	void InsertNodeNext(TListNode* node, TListNode* newNode) {
		if (node == m_pTail) {
			throw InvalidArgumentException("테일 이후에는 노드를 삽입하면 안되요!");
		}

		/*
			[삽입 전]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
			 ↑     ↑
			node node->next

			[삽입 후]
			 ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■ <=> ■
			 ↑     ↑     ↑
			node      node->next
				newNode
		 */

		TListNode* pNodeNext = node->Next;

		ConnectNode(node, newNode);
		ConnectNode(newNode, pNodeNext);
	}

	template <typename... Args>
	void EmplaceBack(Args&&... args) {
		TListNode* pNewNode = EmplaceNewNode(Forward<Args>(args)...);
		InsertNodePrev(m_pTail, pNewNode);
		this->m_iSize++;
	}

	template <typename... Args>
	void EmplaceFront(Args&&... args) {
		TListNode* pNewNode = EmplaceNewNode(Forward<Args>(args)...);
		InsertNodeNext(m_pHead, pNewNode);
		this->m_iSize++;
	}


	virtual void PopFront() {
		this->ThrowIfNoElements();
		
		TListNode* pDel = m_pHead->Next;
		ConnectNode(m_pHead, pDel->Next);
		delete pDel;
		this->m_iSize--;
	}

	virtual void PopBack() {
		this->ThrowIfNoElements();

		TListNode* pDel = m_pTail->Previous;
		ConnectNode(pDel->Previous, m_pTail);
		delete pDel;
		this->m_iSize--;
	}

	virtual T& Front() const {
		this->ThrowIfNoElements();
		return m_pHead->Next->Value;
	}

	virtual T& Back() const {
		this->ThrowIfNoElements();
		return m_pTail->Previous->Value;
	}

	template <typename T>
	TListNode* CreateNewNode(const T& data) {
		TListNode* pNewNode = new TListNode;
		pNewNode->Value = data;
		return pNewNode;
	}

	template <typename T>
	TListNode* CreateNewNode(T&& data) {
		TListNode* pNewNode = new TListNode;
		pNewNode->Value = Move(data);
		return pNewNode;
	}

	template <typename... Args>
	TListNode* EmplaceNewNode(Args&&... args) {
		return new TListNode(Forward<Args>(args)...);
	}

	/// <summary>
	/// 노드끼리 서로 연결한다.
	/// 
	/// lhs의 다음 노드는 rhs로 설정하고
	/// rhs의 이전 노드는 lhs로 설정한다.
	/// </summary>
	void ConnectNode(TListNode* lhs, TListNode* rhs) {
		lhs->Next = rhs;
		rhs->Previous = lhs;
	}
protected:
	TListNode* m_pHead = nullptr;
	TListNode* m_pTail = nullptr;
private:
	TListNode _ValtyHead;
	TListNode _ValtyTail;

	friend class TListCollectionIterator;
};


template <typename T>
ListCollection<T>::~ListCollection() noexcept {
	Clear();

	// 더미노드 제거
	// Memory::Deallocate(m_pHead);
}


} // namespace JCore