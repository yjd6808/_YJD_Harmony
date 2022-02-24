/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Container/Collection.h>
#include <JCore/Container/ListCollectionIterator.h>
#include <JCore/Container/ListNode.h>

namespace JCore {

/*=====================================================================================
									����Ʈ �ݷ���
					     ���Ḯ��Ʈ ��� ť, ����, ���Ḯ��Ʈ �������̽�
						      ���� ���� ����� ���Ḯ��Ʈ�� ����
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
		/* [���̳�� ��� 1]
		m_pHead = Memory::Allocate<TListNode*>(sizeof(TListNode) * 2);	// ���� ���̸� �ѹ��� ��������.
		m_pTail = &m_pHead[1];

		--> ���̸� ���� �����Ҵ��� �ʿ䰡 �����ʳ�..?
		*/

		// [���̳�� ���]
		m_pHead = &_ValtyHead;
		m_pTail = &_ValtyTail;

		// ������ ���̳��� Next�� Previous�� �� ���̹Ƿ�.. ���� TListNode�� Value�� ����Ʈ �����ڸ� ȣ������ �ʿ䰡 ����.
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
		 
		     �� <- �� <=> �� <=> �� <=> �� <=> �� <=> �� <=> �� <=> ��
		     ��                                        ��     ��
		    Head                                    pCur  Tail
			      <----------------------------------->
				           �� ������ �������ش�.


			�׸��� ���� ���� ��� 2��(�Ӹ�, ����)�� �������ش�.
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

	template <typename U = T>
	void CopyFrom(const ListCollection<U>& other) {
		static_assert(IsSameType_v<U, T>, "... U and T is difference type.");
		static_assert(IsAssignable_v<U&, const T&>, "... U cannot be assign to T (T = U is impossible operation)");

		using UListNode = ListNode<U>;
		this->ThrowIfAssignSelf(other);

		TListNode* pCur = m_pHead->Next;
		UListNode* pOtherCur = other.m_pHead->Next;

		// ������ �̹� �޸� �Ҵ�� �༮�� �����͸� �������ش�.
		while (pCur != m_pTail && pOtherCur != other.m_pTail) {
			Memory::PlacementDeallocate(pCur->Value);
			pCur->Value = pOtherCur->Value;
			pCur = pCur->Next;
			pOtherCur = pOtherCur->Next;
		}

		// [Case 1] ���� ������ �Ҵ�� �����Ͱ� �� ���ٸ� 
		//             = pOtherCur�� other.m_pTail�� �����Ǿ� ���� �ݺ����� �������� ���
		// ����� ������ ��� ���� ������ ������ ���ش�.
		//    = pCur->Previous�� ����� ������ ����̰�
		//    = pCur : �����Ǿ���� �����̴�.
		if (this->m_iSize > other.m_iSize) {
			RemoveNodesBetween(pCur->Previous, m_pTail);
		}

		// [Case 2] ���� ������ �����Ͱ� �� ���ٸ�
		while (pOtherCur != other.m_pTail) {
			PushBack(pOtherCur->Value);
			pOtherCur = pOtherCur->Next;
		}

		this->m_iSize = other.m_iSize;
	}

	virtual void CopyFrom(TListCollection&& other) {
		this->ThrowIfAssignSelf(other);
		Clear();

		this->m_Owner = Move(other.m_Owner);

		// ���� ������� ��� other�� ���� ���� ���� ������ �����ϰ� �Ǵµ�
		// �����Ҵ�� �༮�� �ƴϱ� ������ ���߿� ������ ����Ű�� �ȴ�.
		// �׷��� �����Ͱ� ���� ���� �׳� ������ ��
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

		// ������ �̹� �޸� �Ҵ�� �༮�� �����͸� �������ش�.
		while (pCur != m_pTail && pOtherCur != ilist.end()) {
			Memory::PlacementDeallocate(pCur->Value);
			pCur->Value = *pOtherCur;
			pCur = pCur->Next;
			pOtherCur++;
		}

		// [Case 1] ���� ������ �Ҵ�� �����Ͱ� �� ���ٸ� 
		if (this->m_iSize > ilist.size()) {
			RemoveNodesBetween(pCur->Previous, m_pTail);
		}

		// [Case 2] ���� ������ �����Ͱ� �� ���ٸ�
		while (pOtherCur != ilist.end()) {
			PushBack(*pOtherCur);
			pOtherCur++;
		}

		this->m_iSize = ilist.size();
	}

	/// <summary>
	/// exclusiveFirst�� exclusiveLast ��� ���̿� �����ϴ� ������ �����Ѵ�.
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
	/// 
	/// </summary>
	/// <typeparam name="U">
	///		���ø� �Լ� �ν��Ͻ�ȭ�� �����ϱ����� ���ø� �Լ��� �������.
	///		
	///		FindNode �Լ����� T Ÿ�Կ� ���ؼ� ����񱳸� �����ϴµ� ListCollection�� �ڽ� �����̳ʰ� ���ø� �ν��Ͻ�ȭ�� �Ǹ�
	///		�� �����ڰ� ���� ����ü�� Ŭ�����鿡 ���ؼ� ������ �߻���Ų��.
	/// 
	///		�׷��� Remove, FindNode �Լ��� ����� ��쿡�� ���� �񱳰� ���ɿ��ο� ���ؼ� ������ �ٿ쵵�� �Ͽ���.
	/// </typeparam>
	template <typename U = T>
	bool Remove(const U& data) {
		TListNode* pDel = FindNode(data);

		if (pDel == nullptr) {
			return false;
		}

		this->ConnectNode(pDel->Previous, pDel->Next);
		delete pDel;
		this->m_iSize--;
		return true;
	}

	virtual bool Remove(TListCollectionIterator& iter) {
		if (iter.m_pHead != this->m_pHead) {
			throw InvalidOperationException("�ش� ���ͷ����Ͱ� �Ҽӵ� �����̳ʸ� ����� �������ּ���.");
		}

		if (iter.m_pCurrent == this->m_pHead || iter.m_pCurrent == this->m_pTail) {
			throw InvalidOperationException("���ͷ����Ͱ� ó�� �Ǵ� ���� ����Ű�� �ֽ��ϴ�.");
		}

		TListNode* pDel = iter.m_pCurrent;
		iter.m_pCurrent = pDel->Next;
		this->ConnectNode(pDel->Previous, pDel->Next);
		delete pDel;
		this->m_iSize--;
		return true;
	}

	/// <summary>
	/// node �ٷ� ���� newNode�� �����Ѵ�.
	/// </summary>
	/// <param name="tailPrev"></param>
	/// <param name="newNode"></param>
	void InsertNodePrev(TListNode* node, TListNode*  newNode) {
		if (node == m_pHead) {
			throw InvalidArgumentException("��� �������� ��带 �����ϸ� �ȵǿ�!");
		}

		/*
			[���� ��]
			 �� <=> �� <=> �� <=> �� <=> �� <=> ��
							         ��     ��
								          node
							      node->prev

			[���� ��]
			 �� <=> �� <=> �� <=> �� <=> �� <=> �� <=> ��
							         ��     ��     ��
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
	/// node �ٷ� ���Ŀ� newNode�� �����Ѵ�.
	/// </summary>
	/// <param name="tailPrev"></param>
	/// <param name="newNode"></param>
	void InsertNodeNext(TListNode* node, TListNode* newNode) {
		if (node == m_pTail) {
			throw InvalidArgumentException("���� ���Ŀ��� ��带 �����ϸ� �ȵǿ�!");
		}

		/*
			[���� ��]
			 �� <=> �� <=> �� <=> �� <=> �� <=> ��
			 ��     ��
			node node->next

			[���� ��]
			 �� <=> �� <=> �� <=> �� <=> �� <=> �� <=> ��
			 ��     ��     ��
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
	/// ��峢�� ���� �����Ѵ�.
	/// 
	/// lhs�� ���� ���� rhs�� �����ϰ�
	/// rhs�� ���� ���� lhs�� �����Ѵ�.
	/// </summary>
	void ConnectNode(TListNode* lhs, TListNode* rhs) {
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
	TListNode _ValtyHead;
	TListNode _ValtyTail;

	friend class TListCollectionIterator;
};


template <typename T>
ListCollection<T>::~ListCollection() noexcept {
	Clear();

	// ���̳�� ����
	// Memory::Deallocate(m_pHead);
}


} // namespace JCore