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
	ListCollection() {
		m_pHead = Memory::Allocate<TListNode*>(sizeof(TListNode) * 2);	// 양쪽 더미를 한번에 생성하자.
		m_pTail = &m_pHead[1];

		// 어차피 더미노드는 Next와 Previous만 쓸 것이므로.. 굳이 TListNode의 Value의 디폴트 생성자를 호출해줄 필요가 없다.
		// Memory::PlacementAllocate(m_pHead[0]);
		// Memory::PlacementAllocate(m_pHead[1]);

		m_pHead->Previous = nullptr;
		m_pTail->Next = nullptr;

		Connect(m_pHead, m_pTail);

		
	}
	ListCollection(std::initializer_list<T> list) {}
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

		Connect(m_pHead, m_pTail);
		this->m_iSize = 0;
	}
protected:
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

		Connect(pNodePrev, newNode);
		Connect(newNode, node);
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

		Connect(node, newNode);
		Connect(newNode, pNodeNext);
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
		ThrowIfNoElements();
		
		TListNode* pDel = m_pHead->Next;
		Connect(m_pHead, pDel->Next);
		delete pDel;
		this->m_iSize--;
	}

	virtual void PopBack() {
		ThrowIfNoElements();

		TListNode* pDel = m_pTail->Previous;
		Connect(pDel->Previous, m_pTail);
		delete pDel;
		this->m_iSize--;
	}

	virtual T& Front() const {
		ThrowIfNoElements();
		return m_pHead->Next->Value;
	}

	virtual T& Back() const {
		ThrowIfNoElements();
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
	void Connect(TListNode* lhs, TListNode* rhs) {
		lhs->Next = rhs;
		rhs->Previous = lhs;
	}
protected:
	void ThrowIfNoElements() const {
		if (this->m_iSize == 0) {
			throw InvalidOperationException("데이터가 없습니다.");
		}
	}



protected:
	TListNode* m_pHead = nullptr;
	TListNode* m_pTail = nullptr;

	friend class TListCollectionIterator;
};


template <typename T>
ListCollection<T>::~ListCollection() noexcept {
	Clear();

	// 더미노드 제거
	Memory::Deallocate(m_pHead);
}


} // namespace JCore