/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Memory.h>

#include <JCore/Container/Iterable.h>
#include <JCore/Container/Iterator.h>
#include <JCore/Container/CollectionExtension.h>


namespace JCore {

enum class CollectionType
{
	Array,
	List,
	Map,
	Stream,
	KeyCollection,
	ValueCollection
};

enum class ContainerType {
	ArrayQueue,
	ArrayStack,
	Vector,
	ListQueue,
	ListStack,
	LinkedList,
	HashMap,
	TreeMap,
	ReferenceStream,
	HashMapKeyCollection,
	HashMapValueCollection
};

/*=====================================================================================
									콜렉션
							모든 컨테이너의 기본 인터페이스
							 
							         자식
							  1. ListCollection<T>
							  2. ArrayCollection<T>
							  3. MapCollection<T>
				
=====================================================================================*/

template <typename T>
class Collection : public Iterable<T>
{
	using TCollectionExtension	= CollectionExtension<T>;
	using TIterable				= Iterable<T>;
	using TCollection			= Collection<T>;
	using TEnumerator			= Enumerator<T>;
public:
	Collection(CollectionType collectionType, ContainerType containerType) 
		: TIterable(), 
		m_eCollectionType(collectionType),
		m_eContainerType(containerType),
		m_Owner(this, true)
	{
	}

	~Collection() noexcept override { 
		Memory::PlacementDeallocate(m_Owner);
		if (m_bExtensionable)
			Memory::PlacementDeallocate(m_Extension);
	}
protected:
	inline VoidOwner& GetOwner() const { return const_cast<VoidOwner&>(m_Owner); }
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

	TCollectionExtension& Extension() {
		if (!m_bExtensionable) {
			Memory::PlacementAllocate(m_Extension, this);
			m_bExtensionable = true;
		}
		return m_Extension;
	}
protected:
	void ThrowIfAssignSelf(const TCollection& other) {
		if (this == &other) {
			throw InvalidArgumentException("자기 자신에게 대입할 수 없습니다.");
		}
	}

	void ThrowIfNoElements() const {
		if (this->m_iSize == 0) {
			throw InvalidOperationException("데이터가 없습니다.");
		}
	}
protected:
	// @참고 : https://stackoverflow.com/questions/4672438/how-to-access-protected-method-in-base-class-from-derived-class
	// 부모의 자식1과 자식2가 있을때 자식1이 자식2의 부모 정보를 들고 올 수 없는데 이를 우회할 수 있는 방법
	// 컨테이너 타입이 뭔지 public으로 굳이 공개할 필요는 없는데 자식 클래스에서 부모의 protected 멤버 변수에 접근하고 싶을 때 사용
	static CollectionType GetCollectionType(const TCollection& collection) {
		return collection.m_eCollectionType;
	}

	static ContainerType GetContainerType(const TCollection& collection) {
		return collection.m_eContainerType;
	}
protected:
	CollectionType m_eCollectionType;
	ContainerType m_eContainerType;
	int m_iSize = 0;
	union { VoidOwner m_Owner; };
private:
	bool m_bExtensionable = false;
	union { TCollectionExtension m_Extension; };	// 포인터 참조용
};

} // namespace JCore