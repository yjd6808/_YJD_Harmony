/*
	작성자 : 윤정도
*/

#pragma once

#include "jc/Container/ListCollection.h"

NS_JC_BEGIN

template <typename T, typename TAllocator = CDefaultAllocator>
class ListQueue;

template <typename T, typename TAllocator>
class ListQueue	: public ListCollection<T, TAllocator>
{
	using TListCollection			= ListCollection<T, TAllocator>;
	using TListCollectionIterator	= ListCollectionIterator<T, TAllocator, false>;
	using TListQueue				= ListQueue<T, TAllocator>;
public:
	ListQueue() : TListCollection() {}

	ListQueue(const TListQueue& _other) : TListCollection(_other) {}

	ListQueue(TListQueue&& _other) noexcept : TListCollection(Move(_other)) {}

	ListQueue(std::initializer_list<T> _ilist) : TListCollection(_ilist) {}

	~ListQueue() noexcept {}
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

	void Enqueue(const T& _data) {
		TListCollection::PushBack(_data);
	}

	void Enqueue(T&& _data) {
		TListCollection::PushBack(Move(_data));
	}

	template <typename TCollection>
	void EnqueueAll(const TCollection& _collection) {
		TListCollection::PushBackAll(_collection);
	}

	template <typename... Args>
	void Emplace(Args&&... _args) {
		TListCollection::EmplaceBack(Forward<Args>(_args)...);
	}

	void Dequeue() {
		TListCollection::PopFront();
	}

	bool Dequeue(T* _pOut) {
		return TListCollection::PopFront(_pOut);
	}

	T& Front() const {
		return TListCollection::Front();
	}

	TListCollectionIterator Begin() const {
		return TListCollectionIterator(const_cast<TListCollection*>(static_cast<const TListCollection*>(this)), this->pHead_);
	}

	TListCollectionIterator End() const {
		return TListCollectionIterator(const_cast<TListCollection*>(static_cast<const TListCollection*>(this)), this->pTail_);
	}
};

NS_END
