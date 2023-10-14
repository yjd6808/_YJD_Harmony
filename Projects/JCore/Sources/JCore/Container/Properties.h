/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:01:06 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Collection.h>
#include <JCore/Container/HashTable.h>
#include <JCore/Container/Property.h>
#include <JCore/Container/PropertiesIterator.h>

NS_JC_BEGIN

template <typename TKey = int, typename TAllocator = DefaultAllocator>
class Properties : Collection<Pair<TKey, PropertyBase*>, TAllocator>
{
	using THashTable = HashTable<TKey, PropertyBase*, TAllocator>;
	using TPair = Pair<TKey, PropertyBase*>;
	using TIterator = Iterator<TPair, TAllocator>;
	using TPropertiesIterator = PropertiesIterator<TKey, TAllocator>;
	using TNameMap = HashTable<TKey, const char*, TAllocator>;
public:
	Properties(int capacity = 16) : m_hProperties(capacity) {}
	~Properties() override { m_hProperties.ForEachValueDelete(); }

	template <typename Ky>
	PropertyBase* Add(Ky&& propertyKey, PropertyType_t type) {

		if (!PropertyType::CanConstruct[type]) {
			_LogWarn_("%s 타입의 프로퍼티는 생성이 불가능합니다.", PropertyType::Name[type]);
			return nullptr;
		}

		PropertyBase** ppProperty = m_hProperties.Find(Forward<Ky>(propertyKey));
		if (ppProperty != nullptr) {
			_LogWarn_("%s 프로퍼티에 %s타입으로 이미 사용중입니다.", GetPropertyName(propertyKey), PropertyType::Name[(*ppProperty)->GetType()]);
			return nullptr;
		}

		IPropertyFactory* pFactory = PropertyStatics::Factorys[type];
		if (pFactory == nullptr) {
			_LogWarn_("%s 타입의 프로퍼티 팩토리가 없습니다.", PropertyType::Name[type]);
			return nullptr;
		}

		PropertyBase* pNewProperty = pFactory->CreateInstance();
		m_hProperties.Insert(Forward<Ky>(propertyKey), pNewProperty);
		return pNewProperty;
	}

	template <typename Ky, typename Ty>
	PropertyBase* Add(Ky&& propertyKey, PropertyType_t type, Ty&& defaultValue) {
		PropertyBase* pNewProperty = AddProperty(Forward<Ky>(propertyKey), type);
		pNewProperty->Set(Forward<Ty>(defaultValue));
		return pNewProperty;
	}

	template <typename Ky>
	PropertyBase* Get(Ky&& propertyKey) const {
		PropertyBase** ppProperty = m_hProperties.Find(Forward<Ky>(propertyKey));
		if (ppProperty != nullptr) {
			return nullptr;
		}
		return *ppProperty;
	}

	template <typename Ky>
	void Remove(Ky&& propertyKey) {
		return m_hProperties.Remove(Forward<Ky>(propertyKey));
	}

	// 모든 원소 제거
	void Clear() {
		m_hProperties.ForEachValueDelete();
		m_hProperties.Clear();
	}

	// 모든 프로퍼티 값들을 초기 생성 당시 값(0, 문자열의 경우 nullptr)으로 초기화
	void InitializeAllValues() {
		m_hProperties.ForEachValue([](PropertyBase* prop) { prop->SetInitialValue(); });
	}

	template <typename Ky>
	PropertyBase& operator[](Ky&& propertyKey) const {
		PropertyBase** ppProperty = m_hProperties.Find(Forward<Ky>(propertyKey));

		if (ppProperty == nullptr) {
			DebugAssert(false);
			return Property<int>::Default;
		}

		return **ppProperty;
	}

	bool IsEmpty() const override { return m_hProperties.Size() == 0; }
	int Size() const override { return m_hProperties.Size(); }
	ContainerType GetContainerType() override { return ContainerType::Properties; }
	CollectionType GetCollectionType() override { return CollectionType::Properties; }

	SharedPtr<TIterator> Begin() const override {
		return MakeShared<TPropertiesIterator, TAllocator>(
				this->GetOwner(),
				m_hProperties.m_pHeadBucket,
				0
			);
	}

	SharedPtr<TIterator> End() const override {
		return MakeShared<TPropertiesIterator, TAllocator>(
				this->GetOwner(), 
				m_hProperties.m_pTailBucket,
				m_hProperties.m_pTailBucket ? m_hProperties.m_pTailBucket->Size - 1 : -1
			);
	}

	static void SetPropertyNameMap(const TNameMap& nameMap) { PropertyNameMap = nameMap; }
	static void SetPropertyNameMap(TNameMap&& nameMap)		{ PropertyNameMap = Move(nameMap); }

	template <typename Ky>
	static const char* GetPropertyName(Ky&& propertyKey) {
		const char** ppszName = PropertyNameMap.Find(Forward<Ky>(propertyKey));
		if (ppszName == nullptr) {
			return "프로퍼티명 없음";
		}
		return *ppszName;
	}

	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		m_hProperties.ForEach(Forward<Consumer>(consumer));
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& consumer) {
		m_hProperties.ForEachKey(Forward<Consumer>(consumer));
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& consumer) {
		m_hProperties.ForEachValue(Forward<Consumer>(consumer));
	}
private:
	THashTable m_hProperties;

	inline static TNameMap PropertyNameMap;

	friend class TPropertiesIterator;
};

NS_JC_END