/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:03:08 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/PropertyType.h>
#include <JCore/Container/PropertyTypeTraits.h>

NS_JC_BEGIN

void LogLossDataPossible(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType);
void LogImpossibleOperation(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType);
void LogUnsafeOperation(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType);
void LogUnknownOperation(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType);

template <typename Ty>
struct IPropertyValueBinaryOperator;
struct IPropertyValueUnaryOperator;
struct IPropertyValueBinaryOperatorSelector;
struct PropertyBase;



struct JCORE_NOVTABLE IPropertyValueBinaryOperatorSelector
{
	virtual ~IPropertyValueBinaryOperatorSelector() = default;
	virtual void Select(int* decayedLhs, int* decayedRhs, PropertyBinaryOperatorType_t binaryOperator) = 0;
};


struct JCORE_NOVTABLE IPropertyValueUnaryOperator
{
	virtual ~IPropertyValueUnaryOperator() = default;
	virtual void PrefixIncrement(PropertyBase* property) = 0;
	virtual void PrefixDecrement(PropertyBase* property) = 0;
	virtual Int64 PostfixIncrement(PropertyBase* property) = 0;
	virtual Int64 PostfixDecrement(PropertyBase* property) = 0;
};

template <typename Ty>
struct JCORE_NOVTABLE IPropertyValueBinaryOperator
{
	using ValTy = Ty;
	static constexpr PropertyType_t LeftOperandType = PropertyTypeGetter<Ty>::Type;

	virtual ~IPropertyValueBinaryOperator() = default;
	virtual void Operate(Ty& lhs, const Int64& rhs)		= 0;
	virtual void Operate(Ty& lhs, const Int64U& rhs)	= 0;
	virtual void Operate(Ty& lhs, const Int& rhs)		= 0;
	virtual void Operate(Ty& lhs, const Int32U& rhs)	= 0;
	virtual void Operate(Ty& lhs, const Int32L& rhs)	= 0;
	virtual void Operate(Ty& lhs, const Int32UL& rhs)	= 0;
	virtual void Operate(Ty& lhs, const Int16& rhs)		= 0;
	virtual void Operate(Ty& lhs, const Int16U& rhs)	= 0;
	virtual void Operate(Ty& lhs, const Int8& rhs)		= 0;
	virtual void Operate(Ty& lhs, const Int8U& rhs)		= 0;
	virtual void Operate(Ty& lhs, const Float& rhs)		= 0;
	virtual void Operate(Ty& lhs, const Double& rhs)	= 0;
	virtual void Operate(Ty& lhs, const LDouble& rhs)	= 0;
	virtual void Operate(Ty& lhs, const String& rhs)	= 0;
	virtual void Operate(Ty& lhs, char* rhs)			= 0;
};

NS_JC_END