/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 5:49:10 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Collection.h>
#include <JCore/Container/SetCollectionIterator.h>

NS_JC_BEGIN

/*=====================================================================================
									맵 콜렉션
						  트리 맵, 해쉬 맵 공통 인터페이스 정의
=====================================================================================*/

template <typename TKey, typename TAllocator>
class JCORE_NOVTABLE SetCollection : public Collection<TKey, TAllocator>
{
	using TCollection = Collection<TKey, TAllocator>;
	using TSetCollectionIterator = SetCollectionIterator<TKey, TAllocator>;
public:
	SetCollection() = default;
	~SetCollection() noexcept override = 0;
public:
	virtual bool Insert(const TKey& key) = 0;
	virtual bool Insert(TKey&& key) = 0;
	virtual void Clear() = 0;
	virtual bool Exist(const TKey& key) const = 0;
	virtual bool Remove(const TKey& key) = 0;

	CollectionType GetCollectionType() override { return CollectionType::Set; }
};

template <typename TKey, typename TAllocator>
SetCollection<TKey, TAllocator>::~SetCollection() noexcept {}

NS_JC_END