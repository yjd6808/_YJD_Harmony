/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:14:58 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/PropertyTypeTraits.h>
#include <JCore/Container/PropertyStatics.h>

NS_JC_BEGIN

struct PropertyBase
{
	virtual ~PropertyBase() = default;
	virtual PropertyType_t GetType() const = 0;
	virtual const char* GetTypeName() const = 0;
	virtual int* GetDecayedValue() const = 0;

	virtual void Operate(PropertyType_t argumentType, int* decayedArgument, PropertyBinaryOperatorType_t operatorType) = 0;

	static void LogGettingMismatchedType(PropertyType_t lhs, PropertyType_t rhs);
	static void LogConversionFailed(PropertyType_t to, PropertyType_t from);

	template <typename T>
	T As() const {
		constexpr PropertyType_t eGetType = PropertyTypeGetter<T>::Type;
		static_assert(PropertyType::CanBeLeftOperand[eGetType], "... GetValue<T> failed T cannot be left operand");
		using TProperty = Property<T>;
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
	T& Ref();

	template <typename TVal>
	void Set(const TVal& other) {
		using TDesc = PropertyTypeDescription<TVal>;
		static_assert(TDesc::Type != PropertyType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::Type, (int*)&other, PropertyBinaryOperatorType::Store);
	}

	template <typename TVal>
	void Set(TVal&& other) {
		using TDesc = PropertyTypeDescription<TVal>;
		static_assert(TDesc::Type != PropertyType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::ArgumentTyp, (int*)&other, PropertyBinaryOperatorType::Move);
	}

	template <typename TVal>
	PropertyBase& operator=(const TVal& other) {
		using TDesc = PropertyTypeDescription<TVal>;
		static_assert(TDesc::Type != PropertyType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::Type, (int*)&other, PropertyBinaryOperatorType::Store);
		return *this;
	}

	template <typename TVal>
	PropertyBase& operator=(TVal&& other) {
		using TDesc = PropertyTypeDescription<TVal>;
		static_assert(TDesc::Type != PropertyType::Unknown, "... right operand is unknwon argument type");
		Operate(TDesc::Type, (int*)&other, PropertyBinaryOperatorType::Move);
		return *this;
	}

	PropertyBase& operator=(const PropertyBase& other) {
		Operate(other.GetType(), other.GetDecayedValue(), PropertyBinaryOperatorType::Store);
		return *this;
	}
	PropertyBase& operator=(PropertyBase&& other) noexcept {
		Operate(other.GetType(), other.GetDecayedValue(), PropertyBinaryOperatorType::Move);
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
	using TDesc = PropertyTypeDescription<TVal>;																					\
	static_assert(PropertyType::CanBeRightOperand[TDesc::Type], "... right operand cannot perfrom operation:" #op);			\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;	\
	if constexpr (PropertyType::IsPtrType[TDesc::Type])																				\
		lhs.Operate(TDesc::Type, (int*)rhs, eOperatorType);																			\
	else																															\
		lhs.Operate(TDesc::Type, (int*)&rhs, eOperatorType);																		\
																																	\
	return lhs;																														\
}

#define SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(op)														\
template <typename TVal>																											\
bool operator##op(PropertyBase& lhs, const TVal& rhs) {																				\
	using TDesc = PropertyTypeDescription<TVal>;																					\
	static_assert(PropertyType::CanBeRightOperand[TDesc::Type], "... right operand cannot perfrom operation:" #op);					\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;	\
	if constexpr (PropertyType::IsPtrType[TDesc::Type])																				\
		lhs.Operate(TDesc::Type, (int*)rhs, eOperatorType);																			\
	else																															\
		lhs.Operate(TDesc::Type, (int*)&rhs, eOperatorType);																		\
	const bool bRet = PropertyStatics::ComparisonResult;																			\
	PropertyStatics::ComparisonResult = false;																						\
	return bRet;																													\
}


#define SG_PROPERTY_GLOBAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(op)																			\
template <typename TVal>																													\
TVal operator##op(const TVal& lhs, const PropertyBase& rhs) {																				\
	using TDesc = PropertyTypeDescription<TVal>;																							\
	static_assert(PropertyType::CanBeLeftOperand[TDesc::Type], "... left operand cannot perfrom operation:" #op);							\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;			\
																																			\
	TVal ret{ lhs };																														\
	if constexpr (PropertyType::IsPtrType[TDesc::Type])																						\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][rhs.GetType()]->Select((int*)ret, rhs.GetDecayedValue(), eOperatorType);		\
	else																																	\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][rhs.GetType()]->Select((int*)&ret, rhs.GetDecayedValue(), eOperatorType);		\
	return ret;																																\
}																																			


#define SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(op)																\
template <typename TVal>																													\
bool operator##op(const TVal& lhs, const PropertyBase& rhs) {																				\
	using TDesc = PropertyTypeDescription<TVal>;																							\
	static_assert(PropertyType::CanBeLeftOperand[TDesc::Type], "... left operand cannot perfrom operation operation:" #op);					\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;			\
																																			\
	if constexpr (PropertyType::IsPtrType[TDesc::Type])																						\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][rhs.GetType()]->Select((int*)lhs, rhs.GetDecayedValue(), eOperatorType);		\
	else																																	\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][rhs.GetType()]->Select((int*)&lhs, rhs.GetDecayedValue(), eOperatorType);		\
	const bool bRet = PropertyStatics::ComparisonResult;																					\
	PropertyStatics::ComparisonResult = false;																								\
	return bRet;																															\
}




#define SG_PROPERTY_GLOBAL_EQUAL_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(op)																	\
template <typename TVal>																													\
TVal& operator##op(TVal& lhs, const PropertyBase& rhs) {																					\
	using TDesc = PropertyTypeDescription<TVal>;																							\
	static_assert(PropertyType::CanBeLeftOperand[TDesc::Type], "... left operand cannot perfrom operation:" #op);							\
	constexpr PropertyBinaryOperatorType_t eOperatorType = PropertyBinaryOperatorTypeGetter<Hasher64<const char*>()(#op)>::Type;			\
																																			\
	if constexpr (PropertyType::IsPtrType[TDesc::Type])																						\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][rhs.GetType()]->Select((int*)lhs, rhs.GetDecayedValue(), eOperatorType);		\
	else																																	\
		PropertyStatics::BinaryOperatorSelectors[TDesc::Type][rhs.GetType()]->Select((int*)&lhs, rhs.GetDecayedValue(), eOperatorType);		\
	return lhs;																																\
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
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(>=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(<=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(<)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_LEFT_OPERAND(>)

SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(==)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(>=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(<=)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(<)
SG_PROPERTY_GLOBAL_COMPARISON_OPERATOR_IMPLEMENATION_RIGHT_OPERAND(>)

NS_JC_END