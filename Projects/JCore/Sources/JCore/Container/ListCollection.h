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
	using TEnumerator				        = Enumerator<T>;
	using TListNode					        = ListNode<T>;
	using TCollection				        = Collection<T>;
	using TListCollection			      = ListCollection<T>;
	using TListCollectionIterator   = ListCollectionIterator<T>;
public:
	ListCollection(ContainerType containerType) : TCollection(CollectionType::List, containerType)  {
		/* [더미노드 방법 1]
		m_pHead = Memory::Allocate<TListNode*>(sizeof(TListNode) * 2);	// 양쪽 더미를 한번에 생성하자.
		m_pTail = &m_pHead[1];

		--> 더미를 굳이 동적할당할 필요가 없지않나..?
		*/

		// [더미노드 대안]
		m_pHead = &m_ValtyHead;
		m_pTail = &m_ValtyTail;

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

	ListCollection(TListCollection&& other, ContainerType containerType) noexcept
		: TListCollection(containerType)
	{
		CopyFrom(Move(other));
	}

	ListCollection(std::initializer_list<T> ilist, ContainerType containerType)
		: TListCollection(containerType)
	{
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

		TListNode* pCur = m_pTail->Previous;

		while (pCur != m_pHead) {
			const TListNode* pTemp = pCur;
			pCur = pCur->Previous;
			pTemp->DeleteSelf();
		}

		ConnectNode(m_pHead, m_pTail);
		this->m_iSize = 0;
	}
protected:

	template <typename U = T>
	void CopyFrom(const ListCollection<U>& other) {
		static_assert(IsSameType_v<U, T>, "... U and T is difference type.");
		static_assert(IsAssignable_v<U&, const T&>, "... U cannot be assign to T (T = U is impossible operation)");

		using UListNode = ListNode<U>;
		this->ThrowIfAssignSelf(other);

		TListNode* pCur = m_pHead->Next;
		UListNode* pOtherCur = other.m_pHead->Next;

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
		

		m_pTail->Previous = other.m_pTail->Previous;
		m_pHead->Next = other.m_pHead->Next;

		m_pTail->Previous->Next = m_pTail;
		m_pHead->Next->Previous = m_pHead;

		this->m_iSize = other.m_iSize;

		ConnectNode(other.m_pHead, other.m_pTail);
		other.m_iSize = 0;
	}

	template <typename U = T>
	void CopyFrom(std::initializer_list<U> ilist) {
		static_assert(IsSameType_v<U, T>, "... U and T is difference type.");
		static_assert(IsAssignable_v<U&, const T&>, "... U cannot be assign to T (T = U is impossible operation)");

		TListNode* pCur = m_pHead->Next;
		auto pOtherCur = ilist.begin();

		// 기존에 이미 메모리 할당된 녀석은 데이터만 복사해준다.
		while (pCur != m_pTail && pOtherCur != ilist.end()) {
			Memory::PlacementDeallocate(pCur->Value);
			pCur->Value = *pOtherCur;
			pCur = pCur->Next;
			++pOtherCur;
		}

		// [Case 1] 만약 기존에 할당된 데이터가 더 많다면 
		if (this->m_iSize > ilist.size()) {
			RemoveNodesBetween(pCur->Previous, m_pTail);
		}

		// [Case 2] 만약 복사할 데이터가 더 많다면
		while (pOtherCur != ilist.end()) {
			PushBack(*pOtherCur);
			++pOtherCur;
		}

		this->m_iSize = ilist.size();
	}

	/// <summary>
	/// exclusiveFirst와 exclusiveLast 노드 사이에 존재하는 노드들을 삭제한다.
	/// </summary>
	void RemoveNodesBetween(TListNode* exclusiveFirst, TListNode* exclusiveLast) {
		TListNode* pDel = exclusiveFirst->Next;

		while (pDel != m_pTail) {
			const TListNode* pTemp = pDel;
			pDel = pDel->Next;
			delete pTemp;
		}

		ConnectNode(exclusiveFirst, exclusiveLast);
	}

	virtual void PushBack(const T& data) {
		TListNode* pNewNode = CreateNewNode(data);
		InsertNodePrev(m_pTail, pNewNode);
		++this->m_iSize;
	}

	virtual void PushBack(T&& data) {
		TListNode* pNewNode = CreateNewNode(Move(data));
		InsertNodePrev(m_pTail, pNewNode);
		++this->m_iSize;
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

		this->ConnectNode(pDel->Previous, pDel->Next);
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
		this->ConnectNode(pDel->Previous, pDel->Next);
		pDel->DeleteSelf();
		--this->m_iSize;
		return true;
	}

	/// <summary>
	/// node 바로 전에 newNode를 삽입한다.
	/// </summary>
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
		++this->m_iSize;
	}

	virtual void PushFront(T&& data) {
		TListNode* pNewNode = CreateNewNode(Move(data));
		InsertNodeNext(m_pHead, pNewNode);
		++this->m_iSize;
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
		++this->m_iSize;
	}

	template <typename... Args>
	void EmplaceFront(Args&&... args) {
		TListNode* pNewNode = EmplaceNewNode(Forward<Args>(args)...);
		InsertNodeNext(m_pHead, pNewNode);
		++this->m_iSize;
	}


	virtual void PopFront() {
		this->ThrowIfNoElements();
		
		TListNode* pDel = m_pHead->Next;
		ConnectNode(m_pHead, pDel->Next);
		pDel->DeleteSelf();
		--this->m_iSize;
	}

	virtual void PopBack() {
		this->ThrowIfNoElements();

		TListNode* pDel = m_pTail->Previous;
		ConnectNode(pDel->Previous, m_pTail);
		pDel->DeleteSelf();
		--this->m_iSize;
	}

	virtual T& Front() const {
		this->ThrowIfNoElements();
		return m_pHead->Next->Value;
	}

	virtual T& Back() const {
		this->ThrowIfNoElements();
		return m_pTail->Previous->Value;
	}

	TListNode* CreateNewNode(const T& data) {
		TListNode* pNewNode = new TListNode();
		pNewNode->Construct(data);
		return pNewNode;
	}

	TListNode* CreateNewNode(T&& data) {
		TListNode* pNewNode = new TListNode();
		pNewNode->Construct(Move(data));
		return pNewNode;
	}

	template <typename... Args>
	TListNode* EmplaceNewNode(Args&&... args) {
		TListNode* pNewNode = new TListNode;
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
		lhs->Next = rhs;
		rhs->Previous = lhs;
	}

	template <typename U = T>
	TListNode* FindNode(const U& data) const {
		TListNode* pCur = m_pHead->Next;
		while (pCur != m_pTail) {
			if (pCur->Value == data) {
				return pCur;
			}
			pCur = pCur->Next;
		}

		return nullptr;
	}

	template <typename Predicate>
	TListNode* FindNodeIf(Predicate predicate) {
		TListNode* pCur = m_pHead->Next;
		while (pCur != m_pTail) {
			if (predicate(pCur->Value)) {
				return pCur;
			}
			pCur = pCur->Next;
		}

		return nullptr;
	}
protected:
	TListNode* m_pHead = nullptr;
	TListNode* m_pTail = nullptr;
private:
	TListNode m_ValtyHead;
	TListNode m_ValtyTail;

	friend class TListCollectionIterator;
};


template <typename T>
ListCollection<T>::~ListCollection() noexcept {
	Clear();

	// 더미노드 제거
	// Memory::Deallocate(m_pHead);
}


} // namespace JCore