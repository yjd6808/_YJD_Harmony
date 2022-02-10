#pragma once

#include <iostream>
#include <utility>
#include <type_traits>
#include <time.h>
#include <chrono>
#include <functional>
#include <mutex>
#include <memory>
#include <queue>
#include <stack>
#include <list>
#include <unordered_map>
#include <atomic>

#include <JCore/Core.h>
#include <JCore/StringUtil.h>
#include <JCore/String.h>
#include <JCore/TypeTraits.h>
#include <JCore/Type.h>
#include <JCore/Random.h>
#include <JCore/Deletor.h>
#include <JCore/StaticString.h>
#include <JCore/Time.h>
#include <JCore/SmartPointer.h>
#include <JCore/Exception.h>
#include <JCore/PointerObserver.h>
#include <JCore/AutoObject.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Container/Collection.h>

using namespace JCore;
using namespace std;


struct LegendBlock
{
	int a;
	int b;

	LegendBlock() { cout << "cons\n"; }
	~LegendBlock() { cout << "dest\n"; }
};


enum class ArrayType {
	ArrayQueue,
	ArrayStack,
	Vector
};

struct ArrayState {
	ArrayType Type;
};

struct ArrayQueueState : ArrayState {
	int* m_Head;
	int* m_Tail;
};

struct Iterator
{
	Iterator() {}
	virtual ~Iterator() {}

	virtual bool HasNext() = 0;
	virtual bool HasPrevious() = 0;
	virtual int& Next() = 0;
	virtual int& Previous() = 0;
};



struct Node
{
	int Data;
	Owner<Node>* Next = nullptr;
	Owner<Node>* Previous = nullptr;

	~Node() {
		cout << "dest\n";
	}
};




struct ArrayIterator : Iterator 
{
	VoidWatcher Watcher;
	int* Current;
	int Position;

	ArrayIterator(int* cur, int position, VoidOwner& owner) {
		Current = cur;
		Position = position;
		Watcher = owner;
	}

	virtual ~ArrayIterator() = 0;
};

struct ListIterator : Iterator
{
	Watcher<Node> Current;

	ListIterator(Owner<Node>* node) {
		if (node != nullptr) {
			Current = *node;
		}
	}

	virtual ~ListIterator() noexcept override {}

	bool HasNext() override {
		return IsValid() && Current.Get()->Next != nullptr;
	}

	bool HasPrevious() override {
		return IsValid() && Current.Get()->Previous != nullptr;
	}

	int& Next() override {
		if (IsValid() == false) {
			throw InvalidOperationException("유효하지 않은 이터레이터입니다. 기존 컨테이너가 사라졌거나 감시중인 데이터가 소실되었습니다.");
		}

		
		if (Current.Get()->Next != nullptr) {
			Current = *Current.Get()->Next;
		} else {
			Current = nullptr;
		}

		return Current.Get()->Data;
	}

	int& Previous() override {
		if (IsValid() == false) {
			throw InvalidOperationException("유효하지 않은 이터레이터입니다. 기존 컨테이너가 사라졌거나 감시중인 데이터가 소실되었습니다.");
		}

		int& Data = Current.Get()->Data;
		if (Current.Get()->Previous != nullptr) {
			Current = *Current.Get()->Previous;
		} else {
			Current = nullptr;
		}
		return Data;
	}

	bool IsValid() {
		return Current.Exist();
	}
};

struct ArrayQueueIterator : ArrayIterator
{
	ArrayQueueState* State;

	ArrayQueueIterator(int* cur, int Position, VoidOwner& owner, ArrayQueueState* state) : ArrayIterator(cur, Position, owner) {
		State = state;
	}

	~ArrayQueueIterator() override {

	}

	bool HasNext() override {
		return true;
	}

	bool HasPrevious() override {
		return true;
	}

	int& Next() override {
		return *Current;
	}

	int& Previous() override {
		return *Current;
	}

	bool Valid() const {
		return false;
	}

	friend class ArrayQueue;
};

struct ListQueueIterator : ListIterator
{
	ListQueueIterator(Owner<Node>* node) : ListIterator(node) {}
	~ListQueueIterator() noexcept override  {}
};


struct Array {};
struct ArrayQueue : Array {
	int* m_pBuffer = nullptr;
	int m_Head = 0;
	int m_Tail = 0;

	ArrayQueueState* m_pState;
	union { VoidOwner owner; };	// 디폴트 생성자가 없기때문에 유니온으로 감싸서 호출안되도록 함.

	ArrayQueue(int capacity = 600000) {
		m_pBuffer = new int[capacity];
		m_pState = new ArrayQueueState{ ArrayType::ArrayQueue, &m_Head, &m_Tail };
		::new (&owner) VoidOwner(m_pState);	// 수동 생성자 호출
	}

	~ArrayQueue() {
		delete[] m_pBuffer;
		owner.~VoidOwner();
	}

	void Enqueue(int data) {
		m_pBuffer[m_Tail++] = data;
	}

	int&& Dequeue() {
		return Move(m_pBuffer[m_Head++]);
	}

	ArrayQueueIterator Begin() {
		return ArrayQueueIterator( m_pBuffer + m_Head, m_Head, owner, m_pState);
	}

	ArrayQueueIterator End() {
		return ArrayQueueIterator(m_pBuffer + m_Tail, m_Tail, owner, m_pState);
	}
};


struct ListQueue
{
	int m_Size;
	Owner<Node>* Head;
	Owner<Node>* Tail;

	ListQueue() {
		Owner<Node>* Dummy1 = MakeOwnerPointer<Node>(-1);
		Owner<Node>* Dummy2 = MakeOwnerPointer<Node>(-2);

		m_Size = 0;
		Head = Dummy1;
		Tail = Dummy2;

		SetNextNode(Head, Tail);
		SetPrevNode(Tail, Head);
	}

	Owner<Node>* CreateNewNode(int data) {
		return MakeOwnerPointer<Node>(data);
	}

	void Enqueue(int data) {
		Owner<Node>* pNewNode = CreateNewNode(data);
		Owner<Node>* pTailPrevNode = GetPrevNode(Tail);

		SetNextNode(pTailPrevNode, pNewNode);
		SetPrevNode(pNewNode, pTailPrevNode);

		SetNextNode(pNewNode, Tail);
		SetPrevNode(Tail, pNewNode);

		Tail = pNewNode;
		m_Size++;
	}

	int&& Dequeue() {
		if (m_Size == 0) {
			throw InvalidOperationException("데이터가 없습니다.");
		}
		
		Owner<Node>* pDel = GetNextNode(Head);
		Owner<Node>* pDelNext = GetNextNode(pDel);
		Owner<Node>* pDelPrev = GetPrevNode(pDel);

		AutoPointer<Owner<Node>, Deletor<Owner<Node>>> autoObj(pDel);

		SetNextNode(pDelPrev, pDelNext);
		SetPrevNode(pDelNext, pDelPrev);

		m_Size--;
		return Move(GetData(pDel));
	}
	
	bool IsEmpty() const {
		return m_Size == 0;
	}

	void SetNextNode(Owner<Node>* node, Owner<Node>* other) const {
		node->Get()->Next = other;
	}

	void SetPrevNode(Owner<Node>* node, Owner<Node>* other) const {
		node->Get()->Previous = other;
	}

	int& GetData(Owner<Node>* node) const {
		return node->Get()->Data;
	}

	Owner<Node>* GetNextNode(Owner<Node>* node) const {
		return node->Get()->Next;
	}

	Owner<Node>* GetPrevNode(Owner<Node>* node) const {
		return node->Get()->Previous;
	}

	ListQueueIterator Begin() const {
		return ListQueueIterator(Head);
	}

	ListQueueIterator End() const {
		return ListQueueIterator(Tail);
	}
};

struct A
{
	virtual void operator++() = 0;
};

struct B : A
{

	void operator++() override {

	}
};

/*
{
	//ArrayQueue q;
	//q.Enqueue(1);
	//q.Enqueue(2);
	//q.Enqueue(3);

	ListQueue qq;
	qq.Enqueue(1);
	qq.Enqueue(2);
	qq.Enqueue(3);

	ListQueueIterator iter = qq.Begin();

	while (iter.HasNext()) {
		cout << iter.Next() << "\n";
	}

	int out = qq.Dequeue();
	int out2 = qq.Dequeue();
	int out3 = qq.Dequeue();



	Model* pModel = new Model(1);
	AutoPointer<Model, VoidDeletor> d(pModel);
}
*/