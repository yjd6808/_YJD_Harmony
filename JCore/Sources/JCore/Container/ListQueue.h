/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollection.h>
#include <JCore/Container/ListQueueIterator.h>

namespace JCore {

template <typename T>
class ListQueue	: public ListCollection<T>
{
	using TEnumerator			= typename Enumerator<T>;
	using TCollection			= typename Collection<T>;
	using TListCollection		= typename ListCollection<T>;
	using TListQueue			= typename ListQueue<T>;
	using TListQueueIterator	= typename ListQueueIterator<T>;
public:
	ListQueue() 
		: TListCollection(ContainerType::ListQueue) 
	{
	}

	ListQueue(const TListQueue& other) 
		: TListCollection(other, ContainerType::ListQueue) 
	{
	}

	ListQueue(TListQueue&& other) 
		: TListCollection(Move(other), ContainerType::ListQueue) 
	{
	}

	ListQueue(std::initializer_list<T> ilist)
		: TListCollection(ilist, ContainerType::ListQueue)
	{
	}

	virtual ~ListQueue() noexcept {}
public:
	TListQueue& operator=(const TListQueue& other) {
		this->CopyFrom(other);
		return *this;
	}

	TListQueue& operator=(TListQueue&& other) {
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

	virtual T& Front() const {
		return TListCollection::Front();
	}

	virtual TEnumerator Begin() const {
		return MakeShared<TListQueueIterator>(this->GetOwner(), this->m_pHead->Next);
	}

	virtual TEnumerator End() const {
		return MakeShared<TListQueueIterator>(this->GetOwner(), this->m_pTail);
	}
protected:
	friend class TListQueueIterator;
};

} // namespace JCore