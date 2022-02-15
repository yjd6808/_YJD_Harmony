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
	ListCollection() {
		m_pHead = Memory::Allocate<TListNode*>(sizeof(TListNode) * 2);	// ���� ���̸� �ѹ��� ��������.
		m_pTail = &m_pHead[1];

		// ������ ���̳��� Next�� Previous�� �� ���̹Ƿ�.. ���� TListNode�� Value�� ����Ʈ �����ڸ� ȣ������ �ʿ䰡 ����.
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
	/// ��峢�� ���� �����Ѵ�.
	/// 
	/// lhs�� ���� ���� rhs�� �����ϰ�
	/// rhs�� ���� ���� lhs�� �����Ѵ�.
	/// </summary>
	void Connect(TListNode* lhs, TListNode* rhs) {
		lhs->Next = rhs;
		rhs->Previous = lhs;
	}
protected:
	void ThrowIfNoElements() const {
		if (this->m_iSize == 0) {
			throw InvalidOperationException("�����Ͱ� �����ϴ�.");
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

	// ���̳�� ����
	Memory::Deallocate(m_pHead);
}


} // namespace JCore