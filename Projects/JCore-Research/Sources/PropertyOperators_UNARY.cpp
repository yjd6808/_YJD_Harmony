/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 11:57:40 AM
 * =====================
 *
 */


#include "PropertyOperator.h"
#include "Property.h"
#include "PropertyStatics.h"


// Argument를 Type으로 저장하도록함.
template <PropertyType_t Type>
struct PropertyValueUnaryOperator : IPropertyValueUnaryOperator
{
	void PrefixIncrement(PropertyBase* property) override { _LogWarn_("%s타입은 %s연산이 불가능합니다.", property->GetTypeName(), "Prefix ++"); }
	void PrefixDecrement(PropertyBase* property) override { _LogWarn_("%s타입은 %s연산이 불가능합니다.", property->GetTypeName(), "Prefix --"); }
	Int64 PostfixIncrement(PropertyBase* property) override { _LogWarn_("%s타입은 %s연산이 불가능합니다.", property->GetTypeName(), "Postfix ++"); return 0; }
	Int64 PostfixDecrement(PropertyBase* property) override { _LogWarn_("%s타입은 %s연산이 불가능합니다.", property->GetTypeName(), "Postfix --"); return 0; }
};

template <>
struct PropertyValueUnaryOperator<PropertyType::Int> : IPropertyValueUnaryOperator
{
	void PrefixIncrement(PropertyBase* property) override {
		Int32& val = *(Int32*)property->GetDecayedValue();
		++val;
	}

	void PrefixDecrement(PropertyBase* property) override {
		Int32& val = *(Int32*)property->GetDecayedValue();
		++val;
	}

	Int64 PostfixIncrement(PropertyBase* property) override {
		Int32& val = *(Int32*)property->GetDecayedValue();
		Int32 ret = val;
		++val;
		return Int64(ret);
	}

	Int64 PostfixDecrement(PropertyBase* property) override {
		Int32& val = *(Int32*)property->GetDecayedValue();
		Int32 ret = val;
		--val;
		return Int64(ret);
	}

};

template <>
struct PropertyValueUnaryOperator<PropertyType::Int64> : IPropertyValueUnaryOperator
{
	void PrefixIncrement(PropertyBase* property) override {
		Int64& val = *(Int64*)property->GetDecayedValue();
		++val;
	}

	void PrefixDecrement(PropertyBase* property) override {
		Int64& val = *(Int64*)property->GetDecayedValue();
		--val;
	}

	Int64 PostfixIncrement(PropertyBase* property) override {
		Int64& val = *(Int64*)property->GetDecayedValue();
		Int64 ret = val;
		++val;
		return ret;
	}

	Int64 PostfixDecrement(PropertyBase* property) override {
		Int64& val = *(Int64*)property->GetDecayedValue();
		Int64 ret = val;
		--val;
		return ret;
	}
};

template <PropertyType_t I>
void CreateUnaryOperators() {
	if constexpr (I == PropertyType::Unknown) return;
	else {
		PropertyStatics::UnaryOperators[I] = dbg_new PropertyValueUnaryOperator<I>{};
		CreateUnaryOperators<PropertyType_t(I + 1)>();
	}
}

void InitializePropertyOperatorUnary() {
	CreateUnaryOperators<PropertyType_t(0)>();
}