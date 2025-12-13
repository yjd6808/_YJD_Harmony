/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ListCollection.h>
#include <JCore/Container/ListQueueIterator.h>

NS_JC_BEGIN

template <typename T, typename TAllocator = CDefaultAllocator>
class ListQueue;

template <typename T, typename TAllocator>
class ListQueue	: public ListCollection<T, TAllocator>
{
	using TEnumerator			= Enumerator<T, TAllocator>;
	using TCollection			= Collection<T, TAllocator>;
	using TListCollection		= ListCollection<T, TAllocator>;
	using TListQueue			= ListQueue<T, TAllocator>;
	using TListQueueIterator	= ListQueueIterator<T, TAllocator>;
public:
	ListQueue() : TListCollection() {}

	ListQueue(const TListQueue& _other) : TListCollection(_other) {}

	ListQueue(TListQueue&& _other) noexcept : TListCollection(Move(_other)) {}

	ListQueue(std::initializer_list<T> _ilist) : TListCollection(_ilist) {}

	~ListQueue() noexcept override {}
public:
	TListQueue& operator=(const TListQueue& _other) {
		this->CopyFrom(_other);
		return *this;
	}

	TListQueue& operator=(TListQueue&& _other) noexcept {
		this->CopyFrom(Move(_other));
		return *this;
	}
	
	TListQueue& operator=(std::initializer_list<T> _ilist) {
		this->CopyFrom(_ilist);
		return *this;
	}

	virtual void Enqueue(const T& _data) {
		TListCollection::PushBack(_data);
	}

	virtual void Enqueue(T&& _data) {
		TListCollection::PushBack(Move(_data));
	}

	virtual void EnqueueAll(const TCollection& _collection) {
		TListCollection::PushBackAll(_collection);
	}

	template <typename... Args>
	void Emplace(Args&&... _args) {
		TListCollection::EmplaceBack(Forward<Args>(_args)...);
	}

	virtual void Dequeue() {
		TListCollection::PopFront();
	}

	T& Front() const override {
		return TListCollection::Front();
	}

	TEnumerator Begin() const override {
		return MakeShared<TListQueueIterator, TAllocator>(this->GetOwner(), this->pHead_);
	}

	TEnumerator End() const override {
		return MakeShared<TListQueueIterator, TAllocator>(this->GetOwner(), this->pTail_);
	}

	ContainerType GetContainerType() override { return ContainerType::ListQueue; }
protected:
	friend class TListQueueIterator;
};

NS_JC_END
