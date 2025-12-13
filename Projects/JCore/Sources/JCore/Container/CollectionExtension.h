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
	using TCollectionExtension = CollectionExtension<T, TAllocator>;
	using TCollectionStream = CollectionStream<T, TAllocator>;
	using TIterator = Iterator<T, TAllocator>;
	using TEnumerator = SharedPtr<TIterator>;
	using TCollection = Collection<T, TAllocator>;

public:
	CollectionExtension(TCollection* _pCollection)
	{
		pCollection_ = _pCollection;
	}

	virtual ~CollectionExtension() noexcept
	{
	}

	template <typename Consumer>
	TCollectionExtension& ForEach(Consumer&& _consumer)
	{
		TEnumerator enumerator = pCollection_->Begin();
		while (enumerator->HasNext())
		{
			_consumer(enumerator->Next());
		}
		return *this;
	}

	template <typename IndexConsumer>
	TCollectionExtension& ForEachWithIndex(IndexConsumer&& _consumer)
	{
		TEnumerator enumerator = pCollection_->Begin();
		int index = 0;
		while (enumerator->HasNext())
		{
			_consumer(enumerator->Next(), index);
			++index;
		}
		return *this;
	}

	bool Exist(const T& _value)
	{
		TEnumerator enumerator = pCollection_->Begin();
		while (enumerator->HasNext())
		{
			if (enumerator->Next() == _value)
				return true;
		}
		return false;
	}

	template <typename TPredicate>
	bool ExistIf(TPredicate&& _predicate)
	{
		TEnumerator enumerator = pCollection_->Begin();
		while (enumerator->HasNext())
		{
			if (_predicate(enumerator->Next()))
				return true;
		}
		return false;
	}

	template <typename TPredicate>
	TCollectionStream Filter(TPredicate&& _predicate)
	{
		TCollectionStream collectionStream(pCollection_);
		collectionStream.Filter(Move(_predicate));
		return collectionStream;
	}

	T* First()
	{
		if (pCollection_->Size() == 0)
			return nullptr;

		return AddressOf(pCollection_->Begin()->Next());
	}

	T* Last()
	{
		if (pCollection_->Size() == 0)
			return nullptr;

		return AddressOf(pCollection_->End()->Previous());
	}

	T* IndexOf(int _at)
	{
		int index = 0;

		auto enumerator = pCollection_->Begin();
		while (enumerator->HasNext())
		{
			T& currentValue = enumerator->Next();
			if (index++ == _at)
				return AddressOf(currentValue);
		}
		return nullptr;
	}

	template <typename TPredicate>
	T* FindIf(TPredicate&& _predicate)
	{
		if (pCollection_->Size() == 0)
			return nullptr;

		TEnumerator enumerator = pCollection_->Begin();
		while (enumerator->HasNext())
		{
			T& value = enumerator->Next();
			if (_predicate(value))
				return AddressOf(value);
		}

		return nullptr;
	}

	TCollectionStream Sorted()
	{
		return Sorted(NaturalOrder{});
	}

	template <typename TPredicate>
	TCollectionStream Sorted(TPredicate&& _predicate)
	{
		TCollectionStream collectionStream(pCollection_);
		collectionStream.Sorted(Move(_predicate));
		return collectionStream;
	}

	Vector<T, TAllocator> ToVector()
	{
		Vector<T, TAllocator> vector;
		vector.PushBackAll(*pCollection_);
		return vector;
	}

	LinkedList<T, TAllocator> ToLinkedList()
	{
		LinkedList<T, TAllocator> list;
		list.PushBackAll(*pCollection_);
		return list;
	}

private:
	TCollection* pCollection_;
};

NS_JC_END
