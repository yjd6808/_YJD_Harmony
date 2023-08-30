/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:15:31 PM
 * =====================
 *
 */

#include "Property.h"


void PropertyBase::LogGettingMismatchedType(PropertyArgumentType_t lhs, PropertyArgumentType_t rhs) {
	_LogWarn_(" 대해서 %s 타입의 프로퍼티로 가져올려고 시도했습니다.", PropertyArgumentType::Name[lhs], PropertyArgumentType::Name[rhs]);
}

void PropertyBase::LogConversionFailed(PropertyArgumentType_t to, PropertyArgumentType_t from) {
	_LogWarn_("%s타입의 프로퍼티에 %s 타입의 매개변수를 저장하는데 실패함.", PropertyArgumentType::Name[to], PropertyArgumentType::Name[from]);
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

template <>
Int& PropertyBase::GetValue() const {
	using TProperty = Property<Int>;
	const PropertyArgumentType_t eType = GetArgumentType();
	if (eType != PropertyArgumentType::Int) {
		LogGettingMismatchedType(eType, PropertyArgumentType::Int);
		return TProperty::Default.Value;
	}
	return ((TProperty*)this)->Value;
}
//
//template <>
//Int64& PropertyBase::GetValue() const {
//	using TProperty = Property<Int64>;
//	const PropertyArgumentType_t eType = GetArgumentType();
//	if (eType != PropertyArgumentType::Int64) {
//		LogGettingMismatchedType(eType, PropertyArgumentType::Int64);
//		return TProperty::Default.Value;
//	}
//	return ((TProperty*)this)->Value;
//}
//
//template <>
//Float& PropertyBase::GetValue() const {
//	using TProperty = Property<Float>;
//	const PropertyArgumentType_t eType = GetArgumentType();
//	if (eType != PropertyArgumentType::Float) {
//		LogGettingMismatchedType(eType, PropertyArgumentType::Float);
//		return TProperty::Default.Value;
//	}
//	return ((TProperty*)this)->Value;
//}
//
//template <>
//Double& PropertyBase::GetValue() const {
//	using TProperty = Property<Double>;
//	const PropertyArgumentType_t eType = GetArgumentType();
//	if (eType != PropertyArgumentType::Double) {
//		LogGettingMismatchedType(eType, PropertyArgumentType::Double);
//		return TProperty::Default.Value;
//	}
//	return ((TProperty*)this)->Value;
//}
//
//template <>
//String& PropertyBase::GetValue() const {
//	using TProperty = Property<String>;
//	const PropertyArgumentType_t eType = GetArgumentType();
//	if (eType != PropertyArgumentType::String) {
//		LogGettingMismatchedType(eType, PropertyArgumentType::String);
//		return TProperty::Default.Value;
//	}
//	return ((TProperty*)this)->Value;
//}

