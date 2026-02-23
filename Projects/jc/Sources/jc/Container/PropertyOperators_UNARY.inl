/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 11:57:40 AM
 * =====================
 *
 */

#include "Property.h"
#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

// Argument를 Type으로 저장하도록함.
template <PropertyType_t Type>
struct SPropertyValueUnaryOperator : IPropertyValueUnaryOperator
{
	void PrefixIncrement(PropertyBase* _pProperty) override
	{
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", _pProperty->GetTypeName(), "Prefix ++");
	}

	void PrefixDecrement(PropertyBase* _pProperty) override
	{
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", _pProperty->GetTypeName(), "Prefix --");
	}

	_s64 PostfixIncrement(PropertyBase* _pProperty) override
	{
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", _pProperty->GetTypeName(), "Postfix ++");
		return 0;
	}

	_s64 PostfixDecrement(PropertyBase* _pProperty) override
	{
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", _pProperty->GetTypeName(), "Postfix --");
		return 0;
	}
};



template <>
struct SPropertyValueUnaryOperator<PropertyType::_s32> : IPropertyValueUnaryOperator
{
	void PrefixIncrement(PropertyBase* _pProperty) override
	{
		_s32& value = *(_s32*)_pProperty->GetDecayedValue();
		++value;
	}

	void PrefixDecrement(PropertyBase* _pProperty) override
	{
		_s32& value = *(_s32*)_pProperty->GetDecayedValue();
		++value;
	}

	_s64 PostfixIncrement(PropertyBase* _pProperty) override
	{
		_s32& value = *(_s32*)_pProperty->GetDecayedValue();
		_s32 result = value;
		++value;
		return static_cast<_s64>(result);
	}

	_s64 PostfixDecrement(PropertyBase* _pProperty) override
	{
		_s32& value = *(_s32*)_pProperty->GetDecayedValue();
		_s32 result = value;
		--value;
		return static_cast<_s64>(result);
	}
};


template <>
struct SPropertyValueUnaryOperator<PropertyType::_s64> : IPropertyValueUnaryOperator
{
	void PrefixIncrement(PropertyBase* _pProperty) override
	{
		_s64& value = *(_s64*)_pProperty->GetDecayedValue();
		++value;
	}

	void PrefixDecrement(PropertyBase* _pProperty) override
	{
		_s64& value = *(_s64*)_pProperty->GetDecayedValue();
		--value;
	}

	_s64 PostfixIncrement(PropertyBase* _pProperty) override
	{
		_s64& value = *(_s64*)_pProperty->GetDecayedValue();
		_s64 result = value;
		++value;
		return result;
	}

	_s64 PostfixDecrement(PropertyBase* _pProperty) override
	{
		_s64& value = *(_s64*)_pProperty->GetDecayedValue();
		_s64 result = value;
		--value;
		return result;
	}
};


template <PropertyType_t TPropertyType>
void CreateUnaryOperators()
{
	if constexpr (TPropertyType == PropertyType::Unknown)
	{
		return;
	}
	else
	{
		PropertyStatics::UnaryOperators[TPropertyType] = dbg_new SPropertyValueUnaryOperator<TPropertyType>{};
		CreateUnaryOperators<PropertyType_t(TPropertyType + 1)>();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void InitializePropertyOperatorUnary()
{
	CreateUnaryOperators<PropertyType_t(0)>();
}

NS_END
