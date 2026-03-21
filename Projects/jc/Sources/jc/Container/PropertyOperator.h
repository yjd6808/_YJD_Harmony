/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:03:08 PM
 * =====================
 *
 */


#pragma once

#include "jc/Container/PropertyType.h"
#include "jc/Container/PropertyTypeTraits.h"

NS_JC_BEGIN

void LogLossDataPossible(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType);
void LogImpossibleOperation(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType);
void LogUnsafeOperation(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType);
void LogUnknownOperation(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType);
void LogOperation(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType);

template <typename Ty>
struct IPropertyValueBinaryOperator;
struct IPropertyValueUnaryOperator;
struct IPropertyValueBinaryOperatorSelector;
struct PropertyBase;


struct JC_NOVTABLE IPropertyValueBinaryOperatorSelector
{
	virtual ~IPropertyValueBinaryOperatorSelector() = default;
	virtual void Select(int* _pDecayedLhs, int* _pDecayedRhs, PropertyBinaryOperatorType_t _binaryOperator) = 0;
};


struct JC_NOVTABLE IPropertyValueUnaryOperator
{
	virtual ~IPropertyValueUnaryOperator() = default;
	virtual void PrefixIncrement(PropertyBase* _pProperty) = 0;
	virtual void PrefixDecrement(PropertyBase* _pProperty) = 0;
	virtual _s64 PostfixIncrement(PropertyBase* _pProperty) = 0;
	virtual _s64 PostfixDecrement(PropertyBase* _pProperty) = 0;
};

template <typename Ty>
struct JC_NOVTABLE IPropertyValueBinaryOperator
{
	using ValTy = Ty;
	static constexpr PropertyType_t LeftOperandType = PropertyTypeGetter<Ty>::Type;

	virtual ~IPropertyValueBinaryOperator() = default;
	virtual void Operate(Ty& _lhs, const _s64& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _u64& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _s32& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _u32& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _s32l& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _u32l& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _s16& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _u16& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const bool& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _s8& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _u8& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _f32& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _f64& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const _f64l& _rhs) = 0;
	virtual void Operate(Ty& _lhs, const String& _rhs) = 0;
	virtual void Operate(Ty& _lhs, char* _pRhs) = 0;
};

NS_END
