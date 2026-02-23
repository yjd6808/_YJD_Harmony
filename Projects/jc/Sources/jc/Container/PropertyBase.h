/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:14:58 PM
 * =====================
 *
 */


#pragma once

#include <jc/Container/PropertyTypeTraits.h>
#include <jc/Container/PropertyStatics.h>

#include "Property.h"

NS_JC_BEGIN
struct PropertyBase
{
	virtual ~PropertyBase() = default;
	virtual PropertyType_t GetType() const = 0;
	virtual const char* GetTypeName() const = 0;
	virtual int* GetDecayedValue() const = 0;

	virtual void Operate(PropertyType_t _argumentType, int* _pDecayedArgument, PropertyBinaryOperatorType_t _operatorType) const = 0;

	static void LogGettingMismatchedType(PropertyType_t _lhs, PropertyType_t _rhs);
	static void LogConversionFailed(PropertyType_t _to, PropertyType_t _from);


	// 소스파일에 특정 프로퍼티 타입들에 대해서 명시적으로 정의함.
	template <typename T>
	T As() const
	{
		using TProperty = Property<T>;
		constexpr PropertyType_t eParamType = PropertyTypeGetter<T>::Type;
		const PropertyType_t eType = GetType();
		if (eType != eParamType)
		{
			LogGettingMismatchedType(eType, eParamType);
			return TProperty::Default.value_;
		}
		return ((TProperty*)this)->value_;
	}

	template <typename T>
	const T& CRef() const
	{
		using TProperty = Property<T>;
		constexpr PropertyType_t eParamType = PropertyTypeGetter<T>::Type;
		const PropertyType_t eType = GetType();
		if (eType != eParamType)
		{
			LogGettingMismatchedType(eType, eParamType);
			return TProperty::Default.value_;
		}
		return ((TProperty*)this)->value_;
	}

	template <typename T>
	T& Ref();

	/*template <typename T>
	T As() const {
		constexpr PropertyType_t eGetType = PropertyTypeGetter<T>::Type;
		static_assert(jc::IsNaturalType_v<T>, "... T is not natural type");
		static_assert(PropertyType::CAN_BE_LEFT_OPERAND[eGetType], "... As<T>() failed T cannot be left operand. so cannot get value");
		const PropertyType_t eType = GetType();
		T ret{ 0 };
		if (PropertyType::IsConvertiable(eGetType, eType)) {
			PropertyStatics::BinaryOperatorSelectors[eGetType][eType]->Select((int*)&ret, GetDecayedValue(), PropertyBinaryOperatorType::Store);
		} else {
			LogGettingMismatchedType(eType, eGetType);
		}
		return ret;
	}

	
	template <typename T>
	const T& CRef() const {
		constexpr PropertyType_t eGetType = PropertyTypeGetter<T>::Type;
		static_assert(jc::IsNaturalType_v<T>, "... T is not natural type");
		static_assert(PropertyType::CAN_BE_LEFT_OPERAND[eGetType], "... CRef<T>() failed T cannot be left operand. so cannot get value");
		const PropertyType_t eType = GetType();
		if (eGetType != eType) {
			return Property<T>::Default.Value;
		}
		return *(T*)GetDecayedValue();
	}


	template <typename T>
	T& Ref() {
		constexpr PropertyType_t eGetType = PropertyTypeGetter<T>::Type;
		static_assert(jc::IsNaturalType_v<T>, "... T is not natural type");
		static_assert(PropertyType::CAN_BE_LEFT_OPERAND[eGetType], "... Ref<T>() failed T cannot be left operand. so cannot get value");
		const PropertyType_t eType = GetType();
		if (eGetType != eType) {
			return Property<T>::Default.Value;
		}
		return *(T*)GetDecayedValue();
	}*/


	template <typename TVal>
	void Set(const TVal& _other) {
		using TDesc = PropertyTypeDescription<TVal>;
		static_assert(TDesc::Type != PropertyType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::Type, (int*)&_other, PropertyBinaryOperatorType::Store);
	}

	template <typename TVal>
	void Set(TVal&& _other) {
		using TDesc = PropertyTypeDescription<TVal>;
		static_assert(TDesc::Type != PropertyType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::Type, (int*)&_other, PropertyBinaryOperatorType::Move);
	}

	// 프로퍼티 생성 당시의 초기값으로 변경
	virtual void SetInitialValue() = 0;

	template <typename TVal>
	PropertyBase& operator=(const TVal& _other) {
		using TDesc = PropertyTypeDescription<TVal>;
		static_assert(TDesc::Type != PropertyType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::Type, (int*)&_other, PropertyBinaryOperatorType::Store);
		return *this;
	}

	template <typename TVal>
	PropertyBase& operator=(TVal&& _other) {
		using TDesc = PropertyTypeDescription<TVal>;
		static_assert(TDesc::Type != PropertyType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::Type, (int*)&_other, PropertyBinaryOperatorType::Move);
		return *this;
	}

	PropertyBase& operator=(const PropertyBase& _other) {
		Operate(_other.GetType(), _other.GetDecayedValue(), PropertyBinaryOperatorType::Store);
		return *this;
	}
	PropertyBase& operator=(PropertyBase&& _other) noexcept {
		Operate(_other.GetType(), _other.GetDecayedValue(), PropertyBinaryOperatorType::Move);
		return *this;
	}

	bool operator==(const PropertyBase& _other) const {
		Operate(_other.GetType(), _other.GetDecayedValue(), PropertyBinaryOperatorType::Equal);
		return PropertyStatics::ComparisonResult;
	}

	bool operator!=(const PropertyBase& _other) const {
		Operate(_other.GetType(), _other.GetDecayedValue(), PropertyBinaryOperatorType::NotEqual);
		return PropertyStatics::ComparisonResult;
	}

	bool operator>(const PropertyBase& _other) const {
		Operate(_other.GetType(), _other.GetDecayedValue(), PropertyBinaryOperatorType::Greator);
		return PropertyStatics::ComparisonResult;
	}

	bool operator<(const PropertyBase& _other) const {
		Operate(_other.GetType(), _other.GetDecayedValue(), PropertyBinaryOperatorType::Less);
		return PropertyStatics::ComparisonResult;
	}

	bool operator<=(const PropertyBase& _other) const {
		Operate(_other.GetType(), _other.GetDecayedValue(), PropertyBinaryOperatorType::LessEqual);
		return PropertyStatics::ComparisonResult;
	}

	bool operator>=(const PropertyBase& _other) const {
		Operate(_other.GetType(), _other.GetDecayedValue(), PropertyBinaryOperatorType::GreatorEqual);
		return PropertyStatics::ComparisonResult;
	}


	PropertyBase& operator++();
	PropertyBase& operator--();
	_s64 operator++(int);
	_s64 operator--(int);
};


/*
 * const char* const 
 * const char[Size]
 * => 이 문자열 타입들에 대해서는 다른 기본자료형들과 다르게 주소를 얻지 않고 곧바로 int*로 붕괴시킨 후 연산을 수행토록 한다.
 * 왜냐하면 const char[Size]같은 리터럴 문자열의 경우 이중 포인터를 얻는게 불가능하기때문이다.
 */


#define SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(op)\
template <typename TVal>\
PropertyBase& operator##op(PropertyBase& _lhs, const TVal& _rhs) {\
	using TDesc = PropertyTypeDescription<TVal>;\
	static_assert(PropertyType::CAN_BE_RIGHT_OPERAND[TDesc::Type], "... right operand cannot perfrom operation:" #op);\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;\
	if constexpr (PropertyType::IS_PTR_TYPE[TDesc::Type])\
		_lhs.Operate(TDesc::Type, (int*)_rhs, eOperatorType);\
	else\
		_lhs.Operate(TDesc::Type, (int*)&_rhs, eOperatorType);\
\
	return _lhs;\
}


#define SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(op)\
template <typename TVal, jc::DefaultEnableIf_t<!IsSameType_v<NaturalType_t<TVal>, PropertyBase>> = nullptr>\
bool operator##op(PropertyBase& _lhs, const TVal& _rhs) {\
	using TDesc = PropertyTypeDescription<TVal>;\
	static_assert(PropertyType::CAN_BE_RIGHT_OPERAND[TDesc::Type], "... right operand cannot perfrom operation:" #op);\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;\
	if constexpr (PropertyType::IS_PTR_TYPE[TDesc::Type])\
		_lhs.Operate(TDesc::Type, (int*)_rhs, eOperatorType);\
	else\
		_lhs.Operate(TDesc::Type, (int*)&_rhs, eOperatorType);\
	const bool bRet = PropertyStatics::ComparisonResult;\
	PropertyStatics::ComparisonResult = false;\
	return bRet;\
}


#define SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(op)\
template <typename TVal>\
TVal operator##op(const TVal& _lhs, const PropertyBase& _rhs) {\
	using TDesc = PropertyTypeDescription<TVal>;\
	static_assert(PropertyType::CAN_BE_LEFT_OPERAND[TDesc::Type], "... left operand cannot perfrom operation:" #op);\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;\
\
	TVal ret{ _lhs };\
	if constexpr (PropertyType::IS_PTR_TYPE[TDesc::Type])\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][_rhs.GetType()]->Select((int*)ret, _rhs.GetDecayedValue(), eOperatorType);\
	else\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][_rhs.GetType()]->Select((int*)&ret, _rhs.GetDecayedValue(), eOperatorType);\
	return ret;\
}


#define SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(op)\
template <typename TVal>\
bool operator##op(const TVal& _lhs, const PropertyBase& _rhs) {\
	using TDesc = PropertyTypeDescription<TVal>;\
	static_assert(PropertyType::CAN_BE_LEFT_OPERAND[TDesc::Type], "... left operand cannot perfrom operation operation:" #op);\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;\
\
	if constexpr (PropertyType::IS_PTR_TYPE[TDesc::Type])\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][_rhs.GetType()]->Select((int*)_lhs, _rhs.GetDecayedValue(), eOperatorType);\
	else\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][_rhs.GetType()]->Select((int*)&_lhs, _rhs.GetDecayedValue(), eOperatorType);\
	const bool bRet = PropertyStatics::ComparisonResult;\
	PropertyStatics::ComparisonResult = false;\
	return bRet;\
}




#define SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(op)\
template <typename TVal>\
TVal& operator##op(TVal& _lhs, const PropertyBase& _rhs) {\
	using TDesc = PropertyTypeDescription<TVal>;\
	static_assert(PropertyType::CAN_BE_LEFT_OPERAND[TDesc::Type], "... left operand cannot perfrom operation:" #op);\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;\
\
	if constexpr (PropertyType::IS_PTR_TYPE[TDesc::Type])\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][_rhs.GetType()]->Select((int*)_lhs, _rhs.GetDecayedValue(), eOperatorType);\
	else\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][_rhs.GetType()]->Select((int*)&_lhs, _rhs.GetDecayedValue(), eOperatorType);\
	return _lhs;\
}



// SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(+)
// SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(-)
// SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(/)
// SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(*)
// SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(%)

SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(+=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(-=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(/=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(*=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(%=)

SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(+)
SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(-)
SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(/)
SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(*)
SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(%)

SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(+=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(-=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(/=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(*=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(%=)

SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(==)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(!=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(>=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(<=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(<)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(>)

SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(==)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(!=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(>=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(<=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(<)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(>)

NS_END
