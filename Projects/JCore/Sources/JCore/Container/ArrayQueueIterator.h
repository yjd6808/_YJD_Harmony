/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ArrayCollectionIterator.h>

NS_JC_BEGIN


// 전방 선언
							
template <typename, typename>
class ArrayQueue;
class VoidOwner;

template <typename T, typename TAllocator>
class ArrayQueueIterator : public ArrayCollectionIterator<T, TAllocator>
{
	using TArrayCollectionIterator	= ArrayCollectionIterator<T, TAllocator>;
	using TArrayQueue				= ArrayQueue<T, TAllocator>;
public:
	ArrayQueueIterator(VoidOwner& owner, int pos) : TArrayCollectionIterator(owner, pos) {}
	~ArrayQueueIterator() noexcept override {}
public:
	bool HasNext() const override {
		if (!this->IsValid()) {
			return false;
		}

		return IsValidIndex(this->m_iPos);
	}

	bool HasPrevious() const override {
		if (!this->IsValid()) {
			return false;
		}

		int iPos = this->m_iPos - 1;

		if (iPos == -1) {
			iPos = CastArrayQueue()->Capacity() - 1;
		}

		return IsValidIndex(iPos);
	}

	T& Next() override {
		TArrayQueue* pQueue = CastArrayQueue();
		T& val = pQueue->m_pArray[this->m_iPos++];

		if (this->m_iPos == pQueue->Capacity()) {
			this->m_iPos = 0;
		}

		return val;
	}

	T& Previous() override {
		TArrayQueue* pQueue = CastArrayQueue();
		this->m_iPos -= 1;
		if (this->m_iPos == -1) {
			this->m_iPos = pQueue->Capacity() - 1;
		}

		return pQueue->m_pArray[this->m_iPos];
	}

	T& Current() override {
		return CastArrayQueue()->m_pArray[this->m_iPos];
	}

	bool IsEnd() const override {
		return HasNext() == false;
	}

	bool IsBegin() const override {
		return HasPrevious() == false;
	}
protected:

	/// <summary>
	/// [오버라이딩]
	/// - From ArrayCollectionIterator
	/// </summary>

	bool IsValidIndex(int idx) const override {
		TArrayQueue* pQueue = CastArrayQueue();

		if (pQueue->IsEmpty()) {
			return false;
		}

		if (pQueue->IsForwardedHead()) {
			return (idx >= pQueue->m_iHead && idx < pQueue->Capacity()) ||
				   (idx >= 0			   && idx < pQueue->m_iTail);
		} 

		return idx >= pQueue->m_iHead && idx < pQueue->m_iTail;
	}
	


	TArrayQueue* CastArrayQueue() const {
		this->ThrowIfIteratorIsNotValid();
		return this->Watcher.Get<TArrayQueue*>();
	}
};


NS_JC_END