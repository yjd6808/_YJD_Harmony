/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:14:58 PM
 * =====================
 *
 */


#pragma once

#include "PropertyTypeTraits.h"
#include "PropertyStatics.h"

struct PropertyBase
{
	virtual ~PropertyBase() = default;
	virtual PropertyType_t GetType() const = 0;
	virtual PropertyArgumentType_t GetArgumentType() const = 0;
	virtual const char* GetTypeName() const = 0;
	virtual int* GetDecayedValue() const = 0;

	virtual void Operate(PropertyArgumentType_t argumentType, int* decayedArgument, PropertyBinaryOperatorType_t operatorType) = 0;

	static void LogGettingMismatchedType(PropertyArgumentType_t lhs, PropertyArgumentType_t rhs);
	static void LogConversionFailed(PropertyArgumentType_t to, PropertyArgumentType_t from);

	template <typename TVal>
	TVal& GetValue() const;

	template <typename TVal>
	void SetValue(const TVal& other) {
		using TDesc = PropertyArgumentDescription<TVal>;
		static_assert(TDesc::ArgumentType != PropertyArgumentType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::ArgumentType, (int*)&other, PropertyBinaryOperatorType::Store);
	}

	template <typename TVal>
	void SetValue(TVal&& other) {
		using TDesc = PropertyArgumentDescription<TVal>;
		static_assert(TDesc::ArgumentType != PropertyArgumentType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::ArgumentTyp, (int*)&other, PropertyBinaryOperatorType::Move);
	}

	template <typename TVal>
	PropertyBase& operator=(const TVal& other) {
		using TDesc = PropertyArgumentDescription<TVal>;
		static_assert(TDesc::ArgumentType != PropertyArgumentType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::ArgumentType, (int*)&other, PropertyBinaryOperatorType::Store);
		return *this;
	}

	template <typename TVal>
	PropertyBase& operator=(TVal&& other) {
		using TDesc = PropertyArgumentDescription<TVal>;
		static_assert(TDesc::ArgumentType != PropertyArgumentType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::ArgumentType, (int*)&other, PropertyBinaryOperatorType::Move);
		return *this;
	}

	PropertyBase& operator=(const PropertyBase& other) {
		Operate(other.GetArgumentType(), other.GetDecayedValue(), PropertyBinaryOperatorType::Store);
		return *this;
	}
	PropertyBase& operator=(PropertyBase&& other) noexcept {
		Operate(other.GetArgumentType(), other.GetDecayedValue(), PropertyBinaryOperatorType::Move);
		return *this;
	}

	PropertyBase& operator++();
	PropertyBase& operator--();
	Int64 operator++(int);
	Int64 operator--(int);
};


/*
 * const char* const 
 * const char[Size]
 * => 이 문자열 타입들에 대해서는 다른 기본자료형들과 다르게 주소를 얻지 않고 곧바로 int*로 붕괴시킨 후 연산을 수행토록 한다.
 * 왜냐하면 const char[Size]같은 리터럴 문자열의 경우 이중 포인터를 얻는게 불가능하기때문이다.
 */


#define SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(op)															\
template <typename TVal>																											\
PropertyBase& operator##op(PropertyBase& lhs, const TVal& rhs) {																	\
	using TDesc = PropertyArgumentDescription<TVal>;																				\
	static_assert(TDesc::ArgumentType != PropertyArgumentType::Unknown, "... right operand is unknwon argument type");				\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;	\
	if constexpr (TDesc::ArgumentType == PropertyArgumentType::CharPtr)																\
		lhs.Operate(TDesc::ArgumentType, (int*)rhs, eOperatorType);																	\
	else																															\
		lhs.Operate(TDesc::ArgumentType, (int*)&rhs, eOperatorType);																\
																																	\
	return lhs;																														\
}

#define SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(op)														\
template <typename TVal>																											\
bool operator##op(PropertyBase& lhs, const TVal& rhs) {																				\
	using TDesc = PropertyArgumentDescription<TVal>;																				\
	using TArg = typename TDesc::Ty;																								\
	static_assert(TDesc::ArgumentType != PropertyArgumentType::Unknown, "... right operand is unknwon argument type");				\
	TArg* arg = (TArg*)&rhs;																										\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;	\
		if constexpr (TDesc::ArgumentType == PropertyArgumentType::CharPtr)															\
		lhs.Operate(TDesc::ArgumentType, (int*)rhs, eOperatorType);																	\
	else																															\
		lhs.Operate(TDesc::ArgumentType, (int*)&rhs, eOperatorType);																\
	const bool bRet = PropertyStatics::ComparisonResult;																			\
	PropertyStatics::ComparisonResult = false;																						\
	return bRet;																													\
}




//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(+)
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(-)
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(/)
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(*)
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(%)
//

SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(+=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(-=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(/=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(*=)
SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_LEFT_OPERAND(%=)
//
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(+)
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(-)
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(/)
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(*)
//SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(%)
//
//SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(+=)
//SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(-=)
//SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(/=)
//SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(*=)
//SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(%=)

SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(==)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(>=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(<=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(<)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(>)

//SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(==)
//SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(>=)
//SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(<=)
//SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(<)
//SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(>)