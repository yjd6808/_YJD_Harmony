/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Container/Iterable.h>
#include <JCore/Container/Iterator.h>
#include <JCore/Container/CollectionExtension.h>
#include <JCore/Container/CollectionType.h>
#include <JCore/Container/ContainerType.h>

#pragma warning(push)
  #pragma warning(disable: 26495) // Member variable is uninitialized

NS_JC_BEGIN

/*=====================================================================================
									콜렉션
							모든 컨테이너의 기본 인터페이스
							 
							         자식
							  1. ListCollection<T>
							  2. ArrayCollection<T>
							  3. MapCollection<T>
				
=====================================================================================*/

template <typename T, typename TAllocator>
class Collection : public Iterable<T, TAllocator>
{
	using TCollectionExtension	= CollectionExtension<T, TAllocator>;
	using TIterable				= Iterable<T, TAllocator>;
	using TCollection			= Collection<T, TAllocator>;
	using TEnumerator			= Enumerator<T, TAllocator>;
public:
	Collection() 
		: TIterable()
		, m_Owner(this, true)
	{
	}

	~Collection() noexcept override = default;
protected:
	VoidOwner& GetOwner() const { return const_cast<VoidOwner&>(m_Owner); }
public:
	/// <summary>
	/// 가상함수화할 생각 없었는데..
	/// [오버라이딩]
	///  - HashMapKeyCollection
	///  - HashMapValueCollection
	/// </summary>
	virtual bool IsEmpty() const { 
		return m_iSize == 0; 
	}

	virtual int Size() const {
		return m_iSize; 
	}

	virtual bool Valid() {
		return true;
	}

	virtual ContainerType GetContainerType() = 0;
	virtual CollectionType GetCollectionType() = 0;

	TCollectionExtension Extension() {
		return TCollectionExtension(this);
	}
protected:
	VoidOwner m_Owner;
	int m_iSize = 0;
};

NS_JC_END

#pragma warning(pop)