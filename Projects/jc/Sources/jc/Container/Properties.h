/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:01:06 PM
 * =====================
 *
 */


#pragma once

#include "jc/Container/Collection.h"
#include "jc/Container/HashTable.h"
#include "jc/Container/Property.h"
#include "jc/Container/PropertiesIterator.h"
#include "jc/TypeTraits.h"

NS_JC_BEGIN

template <typename TKey = int, typename TAllocator = CDefaultAllocator>
class Properties : Collection<Pair<TKey, PropertyBase*>, TAllocator>
{
	using THashTable = HashTable<TKey, PropertyBase*, TAllocator>;
	using TPair = Pair<TKey, PropertyBase*>;
	using TIterator = Iterator<TPair, TAllocator>;
	using TPropertiesIterator = CPropertiesIterator<TKey, TAllocator>;
	using TNameMap = HashTable<TKey, const char*, TAllocator>;

public:
	Properties(int _capacity = 16)
	: properties_(_capacity)
	{
	}

	~Properties() override
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

		IPropertyFactory* pFactory = PropertyStatics::Factorys[_type];
		if (pFactory == nullptr)
		{
			_LogWarn_("%s 타입의 프로퍼티 팩토리가 없습니다.", PropertyType::NAME[_type]);
			return nullptr;
		}

		PropertyBase** ppProperty = properties_.Find(Forward<Ky>(_propertyKey));
		if (ppProperty != nullptr)
		{
			_LogWarn_("%s 프로퍼티에 %s타입으로 이미 사용중입니다.", jc::StringUtil::ToString(Forward<Ky>(_propertyKey)).SafeSource(), PropertyType::NAME[(*ppProperty)->GetType()]);
			return nullptr;
		}

		PropertyBase* pNewProperty = pFactory->CreateInstance();
		properties_.Insert(Forward<Ky>(_propertyKey), pNewProperty);
		return pNewProperty;
	}

	template <typename Ky, typename Vy>
	PropertyBase* Add(Ky&& _propertyKey, Vy&& _value)
	{
		PropertyType_t type = PropertyTypeGetter<NaturalType_t<Vy>>::Type;
		if (type == PropertyType::Unknown)
		{
			jc_assert("알 수 없는 타입");
			return nullptr;
		}
		PropertyBase* pNewProperty = Add(Forward<Ky>(_propertyKey), type);
		pNewProperty->Set(Forward<Vy>(_value));
		return pNewProperty;
	}

	template <typename Ky, typename Vy>
	PropertyBase* Set(Ky&& _propertyKey, Vy&& _value)
	{
		PropertyBase* pProperty = Get(Forward<Ky>(_propertyKey));
		if (pProperty == nullptr)
		{
			return Add(Forward<Ky>(_propertyKey), Forward<Vy>(_value));
		}

		pProperty->Set(Forward<Vy>(_value));
		return pProperty;
	}

	template <typename Ky> void SetStrng(Ky&& _propertyKey, const String& _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetStrng(Ky&& _propertyKey, String&& _value) { Set(Forward<Ky>(_propertyKey), Move(_value)); }
	template <typename Ky> void SetStrng(Ky&& _propertyKey, const char* _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetFloat(Ky&& _propertyKey, float _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetDouble(Ky&& _propertyKey, double _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetBool(Ky&& _propertyKey, bool _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetU8(Ky&& _propertyKey, _u8 _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetS8(Ky&& _propertyKey, _s8 _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetU16(Ky&& _propertyKey, _u16 _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetS16(Ky&& _propertyKey, _s16 _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetU32(Ky&& _propertyKey, _u32 _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetS32(Ky&& _propertyKey, _s32 _value)  { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetU64(Ky&& _propertyKey, _u64 _value) { Set(Forward<Ky>(_propertyKey), _value); }
	template <typename Ky> void SetS64(Ky&& _propertyKey, _s64 _value) { Set(Forward<Ky>(_propertyKey), _value); }

#undef __DEF_TRY_GETTER
#define __DEF_TRY_GETTER(method_name, type)														\
	template <typename Ky> bool method_name(Ky&& _propertyKey, OUT type& _value) 				\
	{ 																							\
		PropertyBase* pProp = Get(Forward<Ky>(_propertyKey));									\
		if (pProp == nullptr) return false;														\
		if (pProp->GetType() != PropertyTypeGetter<NaturalType_t<type>>::Type)					\
		{ jc_assert_msg(false, "property key type mismatched."); return false; }				\
		_value = pProp->Ref<type>();															\
		return true;																			\
	}

	__DEF_TRY_GETTER(GetString, String)
	__DEF_TRY_GETTER(GetFloat, float)
	__DEF_TRY_GETTER(GetDouble, double)
	__DEF_TRY_GETTER(GetBool, bool)
	__DEF_TRY_GETTER(GetU8, _u8)
	__DEF_TRY_GETTER(GetS8, _s8)
	__DEF_TRY_GETTER(GetU16, _u16)
	__DEF_TRY_GETTER(GetS16, _s16)
	__DEF_TRY_GETTER(GetU32, _u32)
	__DEF_TRY_GETTER(GetS32, _s32)
	__DEF_TRY_GETTER(GetU64, _u64)
	__DEF_TRY_GETTER(GetS64, _s64)

#undef __DEF_GETTER
#define __DEF_GETTER(method_name, type)															\
	template <typename Ky> type method_name(Ky&& _propertyKey) 									\
	{ 																							\
		PropertyBase* pProp = Get(Forward<Ky>(_propertyKey));									\
		if (pProp == nullptr) { return type(); }												\
		if (pProp->GetType() != PropertyTypeGetter<NaturalType_t<type>>::Type)					\
		{ jc_assert_msg(false, "property key type mismatched."); return type(); }				\
		return pProp->Ref<type>();																\
	}

	__DEF_GETTER(GetString, String)
	__DEF_GETTER(GetFloat, float)
	__DEF_GETTER(GetDouble, double)
	__DEF_GETTER(GetBool, bool)
	__DEF_GETTER(GetU8, _u8)
	__DEF_GETTER(GetS8, _s8)
	__DEF_GETTER(GetU16, _u16)
	__DEF_GETTER(GetS16, _s16)
	__DEF_GETTER(GetU32, _u32)
	__DEF_GETTER(GetS32, _s32)
	__DEF_GETTER(GetU64, _u64)
	__DEF_GETTER(GetS64, _s64)

	template <typename Ky> 
	bool ToggleBool(Ky&& _propertyKey)
	{
		PropertyBase* pProp = Get(Forward<Ky>(_propertyKey));
		if (pProp == nullptr) 
		{ 
			SetBool(Forward<Ky>(_propertyKey), true);
			return false;  // 기존에 없었으므로 false 반환
		}
		if (pProp->GetType() != PropertyTypeGetter<NaturalType_t<bool>>::Type)
		{
			jc_assert_msg(false, "property key type mismatched."); 
			return false; 
		}
		bool& v = pProp->Ref<bool>();
		v = !v;
		return true;
	}

	#undef __DEF_GETTER
	#undef __DEF_TRY_GETTER
	
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
		properties_.ForEachValue([](PropertyBase* _prop) { _prop->SetInitialValue(); });
	}

	template <typename Ky>
	PropertyBase& operator[](Ky&& _propertyKey) const
	{
		PropertyBase** ppProperty = properties_.Find(Forward<Ky>(_propertyKey));

		if (ppProperty == nullptr)
		{
			jc_assert(false);
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

NS_END
