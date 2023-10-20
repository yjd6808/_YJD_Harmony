/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:15:31 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Container/Property.h>

NS_JC_BEGIN 

void PropertyBase::LogGettingMismatchedType(PropertyType_t lhs, PropertyType_t rhs) {
	_LogWarn_("%s타입의 프로퍼티를 %s타입의 프로퍼티로 가져올려고 시도했습니다.", PropertyType::Name[lhs], PropertyType::Name[rhs]);
}

void PropertyBase::LogConversionFailed(PropertyType_t to, PropertyType_t from) {
	_LogWarn_("%s타입의 프로퍼티에 %s타입의 매개변수를 저장하는데 실패함.", PropertyType::Name[to], PropertyType::Name[from]);
}


PropertyBase& PropertyBase::operator++() {
	const PropertyType_t eType = GetType();
	IPropertyValueUnaryOperator* pOperator = PropertyStatics::UnaryOperators[eType];
	if (!pOperator) {
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", PropertyType::Name[eType], "prefix ++");
		return *this;
	}

	pOperator->PrefixIncrement(this);
	return *this;
}

PropertyBase& PropertyBase::operator--() {
	const PropertyType_t eType = GetType();
	IPropertyValueUnaryOperator* pOperator = PropertyStatics::UnaryOperators[eType];
	if (!pOperator) {
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", PropertyType::Name[eType], "prefix --");
		return *this;
	}

	pOperator->PrefixDecrement(this);
	return *this;
}

Int64 PropertyBase::operator++(int) {
	const PropertyType_t eType = GetType();
	IPropertyValueUnaryOperator* pOperator = PropertyStatics::UnaryOperators[eType];
	if (!pOperator) {
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", PropertyType::Name[eType], "postfix ++");
		return 0;
	}

	return pOperator->PostfixIncrement(this);
}

Int64 PropertyBase::operator--(int) {
	const PropertyType_t eType = GetType();
	IPropertyValueUnaryOperator* pOperator = PropertyStatics::UnaryOperators[eType];
	if (!pOperator) {
		_LogWarn_("%s타입은 %s연산이 불가능합니다.", PropertyType::Name[eType], "postfix --");
		return 0;
	}

	return pOperator->PostfixDecrement(this);
}


#define SG_PROPERTY_GETTER_IMPLMENTATION(property_type, return_type, function_name, method_specifier)	\
template <>																								\
return_type PropertyBase::function_name() method_specifier {											\
	using TProperty = Property<property_type>;															\
	constexpr PropertyType_t eParamType = PropertyTypeGetter<property_type>::Type;						\
	const PropertyType_t eType = GetType();																\
	if (eType != eParamType) {																			\
		LogGettingMismatchedType(eType, eParamType);													\
		return TProperty::Default.Value;																\
	}																									\
	return ((TProperty*)this)->Value;																	\
}

#define SG_PROPERTY_GETTER_IMPLMENTATION_AS(property_type)	 SG_PROPERTY_GETTER_IMPLMENTATION(property_type, property_type, As, const)
#define SG_PROPERTY_GETTER_IMPLMENTATION_REF(property_type)  SG_PROPERTY_GETTER_IMPLMENTATION(property_type, property_type&, Ref, )
#define SG_PROPERTY_GETTER_IMPLMENTATION_CREF(property_type) SG_PROPERTY_GETTER_IMPLMENTATION(property_type, const property_type&, CRef, const)

SG_PROPERTY_GETTER_IMPLMENTATION_AS(bool)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int8)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int8U)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int16)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int16U)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int32U)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int32L)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int32UL)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int64)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Int64U)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Float)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(Double)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(LDouble)
SG_PROPERTY_GETTER_IMPLMENTATION_AS(String)

SG_PROPERTY_GETTER_IMPLMENTATION_REF(bool)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int8)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int8U)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int16)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int16U)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int32U)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int32L)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int32UL)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int64)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Int64U)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Float)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(Double)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(LDouble)
SG_PROPERTY_GETTER_IMPLMENTATION_REF(String)

SG_PROPERTY_GETTER_IMPLMENTATION_CREF(bool)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int8)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int8U)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int16)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int16U)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int32U)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int32L)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int32UL)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int64)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Int64U)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Float)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(Double)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(LDouble)
SG_PROPERTY_GETTER_IMPLMENTATION_CREF(String)

NS_JC_END