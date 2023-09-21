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


#define SG_PROPERTY_GET_REF_IMPLMENTATION(property_type)							\
template <>																			\
property_type& PropertyBase::Ref() {												\
	using TProperty = Property<property_type>;										\
	const PropertyType_t eType = GetType();											\
	if (eType != PropertyType::property_type) {										\
		LogGettingMismatchedType(eType, PropertyType::property_type);				\
		return TProperty::Default.Value;											\
	}																				\
	return ((TProperty*)this)->Value;												\
}


SG_PROPERTY_GET_REF_IMPLMENTATION(Int8)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int8U)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int16)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int16U)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int32U)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int32L)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int32UL)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int64)
SG_PROPERTY_GET_REF_IMPLMENTATION(Int64U)
SG_PROPERTY_GET_REF_IMPLMENTATION(Float)
SG_PROPERTY_GET_REF_IMPLMENTATION(Double)
SG_PROPERTY_GET_REF_IMPLMENTATION(LDouble)
SG_PROPERTY_GET_REF_IMPLMENTATION(String)

NS_JC_END