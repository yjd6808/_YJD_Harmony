/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Container/Iterator.h>

namespace JCore {

//���� ����
class VoidOwner;	

template <typename>	
class ListCollection;

template <typename, typename> 
struct KeyValuePair;

template <typename TKey, typename TValue>
class MapCollectionIterator : public Iterator<KeyValuePair<TKey, TValue>>
{
	using TKeyValuePair = KeyValuePair<TKey, TValue>;
	using TIterator		= Iterator<TKeyValuePair>;
public:
	MapCollectionIterator(VoidOwner& owner) : TIterator(owner) {}
	~MapCollectionIterator() noexcept override = 0;
};

template <typename TKey, typename TValue>
MapCollectionIterator<TKey, TValue>::~MapCollectionIterator() noexcept {

}

} // namespace JCore