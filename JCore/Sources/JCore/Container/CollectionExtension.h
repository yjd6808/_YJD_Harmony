/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Functional.h>
#include <JCore/Container/CollectionStream.h>

namespace JCore {

template <typename>
class Collection;

template <typename>
class Vector;

template <typename>
class LinkedList;

template <typename T>
class CollectionExtension
{
	using TCollectionExtension  = typename CollectionExtension<T>;
	using TCollectionStream		= typename CollectionStream<T>;
	using TIterator				= typename Iterator<T>;
	using TEnumerator			= typename SharedPointer<TIterator>;
	using TCollection			= typename Collection<T>;

public:
	CollectionExtension(TCollection* collection) {
		m_pCollection = collection;
	}
	virtual ~CollectionExtension() noexcept {}


	template <typename Consumer>
	void ForEach(Consumer consumer) {
		TEnumerator it = m_pCollection->Begin();
		while (it->HasNext()) {
			consumer(it->Next());
		}
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

	template <typename Predicate>
	bool ExistIf(Predicate predicate) {
		TEnumerator it = m_pCollection->Begin();
		while (it->HasNext()) {
			if (predicate(it->Next())) {
				return true;
			}
		}
		return false;
	}

	template <typename Predicate>
	TCollectionStream Filter(Predicate predicate) {
		TCollectionStream collection(m_pCollection);
		collection.Filter(predicate);
		return collection;
	}

	T* First() {
		if (m_pCollection->Size() == 0) {
			return nullptr;
		}

		return Addressof(m_pCollection->Begin()->Next());
	}

	T* Last() {
		if (m_pCollection->Size() == 0) {
			return nullptr;
		}

		return Addressof(m_pCollection->End()->Previous());
	}

	template <typename Predicate>
	T* FindIf(Predicate predicate) {
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

	template <typename Predicate>
	TCollectionStream Sorted(Predicate predicate) {
		TCollectionStream collection(m_pCollection);
		collection.Sorted(predicate);
		return collection;
	}

	Vector<T> ToVector() {
		Vector<T> v;
		v.PushBackAll(*m_pCollection);
		return v;
	}

	LinkedList<T> ToLinkedList() {
		LinkedList<T> l;
		l.PushBackAll(*m_pCollection);
		return l;
	}
private:
	TCollection* m_pCollection;
};

} // namespace JCore