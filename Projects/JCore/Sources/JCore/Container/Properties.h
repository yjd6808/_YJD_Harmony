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

template <typename TKey = int, typename TAllocator = CDefaultAllocator>
class CProperties : Collection<Pair<TKey, PropertyBase*>, TAllocator>
{
	using THashTable = HashTable<TKey, PropertyBase*, TAllocator>;
	using TPair = Pair<TKey, PropertyBase*>;
	using TIterator = Iterator<TPair, TAllocator>;
	using TPropertiesIterator = CPropertiesIterator<TKey, TAllocator>;
	using TNameMap = HashTable<TKey, const char*, TAllocator>;

public:
	CProperties(int _capacity = 16)
		: properties_(_capacity)
	{
	}

	~CProperties() override
	{
		properties_.ForEachValueDelete();
	}

	template <typename Ky>
	PropertyBase* Add(Ky&& _propertyKey, PropertyType_t _type)
	{
		if (!PropertyType::CAN_CONSTRUCT[_type])
		{
			_LogWarn_("%s 타입의 프로퍼티는 생성이 불가능합니다.", PropertyType::NAME[_type]);
			return nullptr;
		}

		PropertyBase** ppProperty = properties_.Find(Forward<Ky>(_propertyKey));
		if (ppProperty != nullptr)
		{
			_LogWarn_("%s 프로퍼티에 %s타입으로 이미 사용중입니다.", GetPropertyName(_propertyKey), PropertyType::NAME[(*ppProperty)->GetType()]);
			return nullptr;
		}

		IPropertyFactory* pFactory = PropertyStatics::Factorys[_type];
		if (pFactory == nullptr)
		{
			_LogWarn_("%s 타입의 프로퍼티 팩토리가 없습니다.", PropertyType::NAME[_type]);
			return nullptr;
		}

		PropertyBase* pNewProperty = pFactory->CreateInstance();
		properties_.Insert(Forward<Ky>(_propertyKey), pNewProperty);
		return pNewProperty;
	}

	template <typename Ky, typename Ty>
	PropertyBase* Add(Ky&& _propertyKey, PropertyType_t _type, Ty&& _defaultValue)
	{
		PropertyBase* pNewProperty = AddProperty(Forward<Ky>(_propertyKey), _type);
		pNewProperty->Set(Forward<Ty>(_defaultValue));
		return pNewProperty;
	}

	template <typename Ky>
	PropertyBase* Get(Ky&& _propertyKey) const
	{
		PropertyBase** ppProperty = properties_.Find(Forward<Ky>(_propertyKey));
		if (ppProperty == nullptr)
		{
			return nullptr;
		}

		return *ppProperty;
	}

	template <typename Ky>
	void Remove(Ky&& _propertyKey)
	{
		return properties_.Remove(Forward<Ky>(_propertyKey));
	}

	// 모든 원소 제거
	void Clear()
	{
		properties_.ForEachValueDelete();
		properties_.Clear();
	}

	// 모든 프로퍼티 값들을 초기 생성 당시 값(0, 문자열의 경우 nullptr)으로 초기화
	void InitializeAllValues()
	{
		properties_.ForEachValue([](PropertyBase* prop) { prop->SetInitialValue(); });
	}

	template <typename Ky>
	PropertyBase& operator[](Ky&& _propertyKey) const
	{
		PropertyBase** ppProperty = properties_.Find(Forward<Ky>(_propertyKey));

		if (ppProperty == nullptr)
		{
			DebugAssert(false);
			return Property<int>::Default;
		}

		return **ppProperty;
	}

	bool IsEmpty() const override
	{
		return properties_.Size() == 0;
	}

	int Size() const override
	{
		return properties_.Size();
	}

	ContainerType GetContainerType() override
	{
		return ContainerType::Properties;
	}

	CollectionType GetCollectionType() override
	{
		return CollectionType::Properties;
	}

	SharedPtr<TIterator> Begin() const override
	{
		return MakeShared<TPropertiesIterator, TAllocator>(
			this->GetOwner(),
			properties_.pHeadBucket_,
			0);
	}

	SharedPtr<TIterator> End() const override
	{
		return MakeShared<TPropertiesIterator, TAllocator>(
			this->GetOwner(),
			properties_.pTailBucket_,
			properties_.pTailBucket_ ? properties_.pTailBucket_->size_ - 1 : -1);
	}

	static void SetPropertyNameMap(const TNameMap& _nameMap)
	{
		PropertyNameMap = _nameMap;
	}

	static void SetPropertyNameMap(TNameMap&& _nameMap)
	{
		PropertyNameMap = Move(_nameMap);
	}

	template <typename Ky>
	static const char* GetPropertyName(Ky&& _propertyKey)
	{
		const char** ppszName = PropertyNameMap.Find(Forward<Ky>(_propertyKey));
		if (ppszName == nullptr)
		{
			return "프로퍼티명 없음";
		}

		return *ppszName;
	}

	template <typename Consumer>
	void ForEach(Consumer&& _consumer)
	{
		properties_.ForEach(Forward<Consumer>(_consumer));
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& _consumer)
	{
		properties_.ForEachKey(Forward<Consumer>(_consumer));
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& _consumer)
	{
		properties_.ForEachValue(Forward<Consumer>(_consumer));
	}

private:
	THashTable properties_;

	inline static TNameMap PropertyNameMap;

	friend class TPropertiesIterator;
};

NS_JC_END
