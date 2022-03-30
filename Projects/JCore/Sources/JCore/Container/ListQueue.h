/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Container/ListCollection.h>
#include <JCore/Container/ListQueueIterator.h>

namespace JCore {

template <typename T>
class ListQueue	: public ListCollection<T>
{
	using TEnumerator			= Enumerator<T>;
	using TCollection			= Collection<T>;
	using TListCollection		= ListCollection<T>;
	using TListQueue			= ListQueue<T>;
	using TListQueueIterator	= ListQueueIterator<T>;
public:
	ListQueue() 
		: TListCollection(ContainerType::ListQueue) 
	{
	}

	ListQueue(const TListQueue& other) 
		: TListCollection(other, ContainerType::ListQueue) 
	{
	}

	ListQueue(TListQueue&& other) noexcept
		: TListCollection(Move(other), ContainerType::ListQueue) 
	{
	}

	ListQueue(std::initializer_list<T> ilist)
		: TListCollection(ilist, ContainerType::ListQueue)
	{
	}

	~ListQueue() noexcept override {}
public:
	TListQueue& operator=(const TListQueue& other) {
		this->CopyFrom(other);
		return *this;
	}

	TListQueue& operator=(TListQueue&& other) noexcept {
		this->CopyFrom(Move(other));
		return *this;
	}
	
	TListQueue& operator=(std::initializer_list<T> ilist) {
		this->CopyFrom(ilist);
		return *this;
	}

	virtual void Enqueue(const T& data) {
		TListCollection::PushBack(data);
	}

	virtual void Enqueue(T&& data) {
		TListCollection::PushBack(Move(data));
	}

	virtual void EnqueueAll(const TCollection& collection) {
		TListCollection::PushBackAll(collection);
	}

	template <typename... Args>
	void Emplace(Args&&... args) {
		TListCollection::EmplaceBack(Forward<Args>(args)...);
	}

	virtual void Dequeue() {
		TListCollection::PopFront();
	}

	T& Front() const override {
		return TListCollection::Front();
	}

	TEnumerator Begin() const override {
		return MakeShared<TListQueueIterator>(this->GetOwner(), this->m_pHead->Next);
	}

	TEnumerator End() const override {
		return MakeShared<TListQueueIterator>(this->GetOwner(), this->m_pTail);
	}
protected:
	friend class TListQueueIterator;
};

} // namespace JCore