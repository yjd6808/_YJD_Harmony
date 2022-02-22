/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/ArrayCollectionIterator.h>

namespace JCore {


// 전방 선언
					class VoidOwner;
template <typename> class ArrayQueue;
template <typename T>
class ArrayQueueIterator : public ArrayCollectionIterator<T>
{
	using TArrayCollectionIterator	= typename ArrayCollectionIterator<T>;
	using TArrayQueue				= typename ArrayQueue<T>;
public:
	ArrayQueueIterator(VoidOwner& owner, int pos) : TArrayCollectionIterator(owner, pos) {}
	virtual ~ArrayQueueIterator() noexcept {}
public:
	virtual bool HasNext() const {
		if (!this->IsValid()) {
			return false;
		}

		return IsValidIndex(this->m_iPos);
	}

	virtual bool HasPrevious() const {
		if (!this->IsValid()) {
			return false;
		}

		int iPos = this->m_iPos - 1;

		if (iPos == -1) {
			iPos = CastArrayQueue()->Capacity() - 1;
		}

		return IsValidIndex(iPos);
	}

	virtual T& Next() {
		TArrayQueue* pQueue = CastArrayQueue();
		T& val = pQueue->m_pArray[this->m_iPos++];

		if (this->m_iPos == pQueue->Capacity()) {
			this->m_iPos = 0;
		}

		return val;
	}

	virtual T& Previous() {
		TArrayQueue* pQueue = CastArrayQueue();
		this->m_iPos -= 1;
		if (this->m_iPos == -1) {
			this->m_iPos = pQueue->Capacity() - 1;
		}

		return pQueue->m_pArray[this->m_iPos];
	}

	virtual bool IsEnd() const {
		return HasNext() == false;
	}

	virtual bool IsBegin() const {
		return HasPrevious() == false;
	}
protected:

	/// <summary>
	/// [오버라이딩]
	/// - From ArrayCollectionIterator
	/// </summary>
	
	virtual bool IsValidIndex(int idx) const {
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
		return this->Watcher.Get<TArrayQueue>();
	}
};


} // namespace JCore