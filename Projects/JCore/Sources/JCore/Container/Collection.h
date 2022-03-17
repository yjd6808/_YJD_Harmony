/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/Type.h>
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
									�ݷ���
							��� �����̳��� �⺻ �������̽�
							 
							         �ڽ�
							  1. ListCollection<T>
							  2. ArrayCollection<T>
							  3. MapCollection<T>
				
=====================================================================================*/

template <typename T>
class Collection : public Iterable<T>
{
	using TCollectionExtension	= typename CollectionExtension<T>;
	using TIterable				= typename Iterable<T>;
	using TCollection			= typename Collection<T>;
	using TEnumerator			= typename Enumerator<T>;
public:
	Collection(CollectionType collectionType, ContainerType containerType) 
		: TIterable(), 
		m_eCollectionType(collectionType),
		m_eContainerType(containerType),
		m_Owner(this, true)
	{
	}
	virtual ~Collection() noexcept { 
		Memory::PlacementDeallocate(m_Owner);
		if (m_bExtensionable)
			Memory::PlacementDeallocate(m_Extension);
	}
protected:
	inline VoidOwner& GetOwner() const { return const_cast<VoidOwner&>(m_Owner); }
public:
	/// <summary>
	/// �����Լ�ȭ�� ���� �����µ�..
	/// [�������̵�]
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
			throw InvalidArgumentException("�ڱ� �ڽſ��� ������ �� �����ϴ�.");
		}
	}

	void ThrowIfNoElements() const {
		if (this->m_iSize == 0) {
			throw InvalidOperationException("�����Ͱ� �����ϴ�.");
		}
	}
protected:
	// @���� : https://stackoverflow.com/questions/4672438/how-to-access-protected-method-in-base-class-from-derived-class
	// �θ��� �ڽ�1�� �ڽ�2�� ������ �ڽ�1�� �ڽ�2�� �θ� ������ ��� �� �� ���µ� �̸� ��ȸ�� �� �ִ� ���
	// �����̳� Ÿ���� ���� public���� ���� ������ �ʿ�� ���µ� �ڽ� Ŭ�������� �θ��� protected ��� ������ �����ϰ� ���� �� ���
	static CollectionType _CollectionType(const TCollection& collection) {
		return collection.m_eCollectionType;
	}

	static ContainerType _ContainerType(const TCollection& collection) {
		return collection.m_eContainerType;
	}
protected:
	CollectionType m_eCollectionType;
	ContainerType m_eContainerType;
	int m_iSize = 0;
	union { VoidOwner m_Owner; };
private:
	bool m_bExtensionable = false;
	union { TCollectionExtension m_Extension; };	// ������ ������
};

} // namespace JCore