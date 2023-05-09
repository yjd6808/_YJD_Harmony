/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Container/CollectionStream.h>

NS_JC_BEGIN

template <typename, typename> class Vector;
template <typename, typename> class LinkedList;
template <typename, typename> class Collection;

template <typename T, typename TAllocator>
class CollectionExtension
{
	using TCollectionExtension  = CollectionExtension<T, TAllocator>;
	using TCollectionStream		= CollectionStream<T, TAllocator>;
	using TIterator				= Iterator<T, TAllocator>;
	using TEnumerator			= SharedPtr<TIterator>;
	using TCollection			= Collection<T, TAllocator>;

public:
	CollectionExtension(TCollection* collection) {
		m_pCollection = collection;
	}
	virtual ~CollectionExtension() noexcept {
		
	}


	template <typename Consumer>
	TCollectionExtension& ForEach(Consumer&& consumer) {
		TEnumerator it = m_pCollection->Begin();
		while (it->HasNext()) {
			consumer(it->Next());
		}
		return *this;
	}

	template <typename IndexConsumer>
	TCollectionExtension& ForEachWithIndex(IndexConsumer&& consumer) {
		TEnumerator it = m_pCollection->Begin();
		int i = 0;
		while (it->HasNext()) {
			consumer(it->Next(), i);
			++i;
		}
		return *this;
	}

	bool Exist(const T& val) {
		TEnumerator it = m_pCollection->Begin();
		while (it->HasNext()) {
			if (it->Next() == val) {
				return true;
			}
		}
		return false;
	}

	template <typename TPredicate>
	bool ExistIf(TPredicate&& predicate) {
		TEnumerator it = m_pCollection->Begin();
		while (it->HasNext()) {
			if (predicate(it->Next())) {
				return true;
			}
		}
		return false;
	}

	template <typename TPredicate>
	TCollectionStream Filter(TPredicate&& predicate) {
		TCollectionStream collection(m_pCollection);
		collection.Filter(Move(predicate));
		return collection;
	}

	T* First() {
		if (m_pCollection->Size() == 0) {
			return nullptr;
		}

		return AddressOf(m_pCollection->Begin()->Next());
	}

	T* Last() {
		if (m_pCollection->Size() == 0) {
			return nullptr;
		}

		return AddressOf(m_pCollection->End()->Previous());
	}

    T* IndexOf(int at) {
	    int idx = 0;

        auto it = m_pCollection->Begin();
        while (it->HasNext()) {
            T& cur = it->Next();
            if (idx++ == at) {
                return AddressOf(cur);
            }
        }
        return nullptr;
	}

	template <typename TPredicate>
	T* FindIf(TPredicate&& predicate) {
		if (m_pCollection->Size() == 0) {
			return nullptr;
		}

		TEnumerator it = m_pCollection->Begin();
		while (it->HasNext()) {
			T& val = it->Next();
			if (predicate(val)) {
				return AddressOf(val);
			}
		}

		return nullptr;
	}



	TCollectionStream Sorted() {
		return Sorted(NaturalOrder{});
	}

	template <typename TPredicate>
	TCollectionStream Sorted(TPredicate&& predicate) {
		TCollectionStream collection(m_pCollection);
		collection.Sorted(Move(predicate));
		return collection;
	}

	Vector<T, TAllocator> ToVector() {
		Vector<T, TAllocator> v;
		v.PushBackAll(*m_pCollection);
		return v;
	}

	LinkedList<T, TAllocator> ToLinkedList() {
		LinkedList<T, TAllocator> l;
		l.PushBackAll(*m_pCollection);
		return l;
	}
private:
	TCollection* m_pCollection;
};

NS_JC_END