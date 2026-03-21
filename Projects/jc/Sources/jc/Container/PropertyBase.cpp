/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:15:31 PM
 * =====================
 *
 */

#include "jc/Container/Property.h"

NS_JC_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PropertyBase::LogGettingMismatchedType(PropertyType_t _lhs, PropertyType_t _rhs)
{
	_LogWarn_("%s타입의 프로퍼티를 %s타입의 프로퍼티로 가져올려고 시도했습니다.", PropertyType::NAME[_lhs], PropertyType::NAME[_rhs]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PropertyBase::LogConversionFailed(PropertyType_t _to, PropertyType_t _from)
{
	_LogWarn_("%s타입의 프로퍼티에 %s타입의 매개변수를 저장하는데 실패함.", PropertyType::NAME[_to], PropertyType::NAME[_from]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
PropertyBase& PropertyBase::operator++()
{
	const PropertyType_t eType = GetType();
	IPropertyValueUnaryOperator* pOperator = PropertyStatics::UnaryOperators[eType];
	if (!pOperator)
	{
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", PropertyType::NAME[eType], "prefix ++");
		return *this;
	}

	pOperator->PrefixIncrement(this);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
PropertyBase& PropertyBase::operator--()
{
	const PropertyType_t eType = GetType();
	IPropertyValueUnaryOperator* pOperator = PropertyStatics::UnaryOperators[eType];
	if (!pOperator)
	{
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", PropertyType::NAME[eType], "prefix --");
		return *this;
	}

	pOperator->PrefixDecrement(this);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
_s64 PropertyBase::operator++(int)
{
	const PropertyType_t eType = GetType();
	IPropertyValueUnaryOperator* pOperator = PropertyStatics::UnaryOperators[eType];
	if (!pOperator)
	{
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", PropertyType::NAME[eType], "postfix ++");
		return 0;
	}

	return pOperator->PostfixIncrement(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
_s64 PropertyBase::operator--(int)
{
	const PropertyType_t eType = GetType();
	IPropertyValueUnaryOperator* pOperator = PropertyStatics::UnaryOperators[eType];
	if (!pOperator)
	{
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", PropertyType::NAME[eType], "postfix --");
		return 0;
	}

	return pOperator->PostfixDecrement(this);
}


#define SG_PROPERTY_GETTER_IMPLMENTATION(property_type, return_type, function_name, method_specifier)\
template <>\
return_type PropertyBase::function_name() method_specifier {\
	using TProperty = Property<property_type>;\
	constexpr PropertyType_t eParamType = PropertyTypeGetter<property_type>::Type;\
	const PropertyType_t eType = GetType();\
	if (eType != eParamType) {\
		LogGettingMismatchedType(eType, eParamType);\
		return TProperty::Default.value_;\
	}\
	return ((TProperty*)this)->value_;\
}

#define SG_PROPERTY_GETTER_IMPLMENTATION_REF(property_type)  SG_PROPERTY_GETTER_IMPLMENTATION(property_type, property_type&, Ref, )
#define SG_PROPERTY_GETTER_IMPLMENTATION_CREF(property_type) SG_PROPERTY_GETTER_IMPLMENTATION(property_type, const property_type&, CRef, const)


SG_PROPERTY_GETTER_IMPLMENTATION_REF(bool)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_s8)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_u8)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_s16)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_u16)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_s32)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_u32)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_s32l)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_u32l)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_s64)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_u64)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_f32)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_f64)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(_f64l)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(String)

SG_PROPERTY_GETTER_IMPLMENTATION_CREF(bool)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_s8)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_u8)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_s16)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_u16)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_s32)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_u32)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_s32l)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_u32l)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_s64)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_u64)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_f32)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_f64)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(_f64l)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(String)

NS_END
