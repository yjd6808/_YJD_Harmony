/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:01:19 PM
 * =====================
 *
 */


#include "PropertyStatics.h"
#include "PropertyType.h"

// Argument를 Type으로 저장하도록함.
template <PropertyType_t Type, PropertyArgumentType_t ArgumentType>
struct PropertyValueBinaryOperatorSelector : IPropertyValueBinaryOperatorSelector
{
	
	using Ty = typename PropertyDataTypeGetter<Type>::Ty;
	using ArgTy = typename PropertyArgumentDataTypeGetter<ArgumentType>::Ty;

	// 올바른 타입정보를 얻지 못한 경우
	static_assert(!IsVoidType_v<Ty>&& !IsVoidType_v<ArgTy>, "... ty and argty must not be void type");

	void Select(int* decayedLhs, int* decayedRhs, PropertyBinaryOperatorType_t binaryOperator) override {
		Ty* lhs = (Ty*)decayedLhs;

		if constexpr (ArgumentType == PropertyArgumentType::CharPtr) {
			ArgTy rhs = (ArgTy)decayedRhs;
			if constexpr (Type == PropertyType::Int)			PropertyStatics::BinaryOperators_Int[binaryOperator]->Operate(*lhs, rhs);
			else if constexpr (Type == PropertyType::Int64)		PropertyStatics::BinaryOperators_Int64[binaryOperator]->Operate(*lhs, rhs);
			else if constexpr (Type == PropertyType::Float)		PropertyStatics::BinaryOperators_Float[binaryOperator]->Operate(*lhs, rhs);
			else if constexpr (Type == PropertyType::Double)	PropertyStatics::BinaryOperators_Double[binaryOperator]->Operate(*lhs, rhs);
			else if constexpr (Type == PropertyType::String)	PropertyStatics::BinaryOperators_String[binaryOperator]->Operate(*lhs, rhs);
		} else {
			ArgTy* rhs = (ArgTy*)decayedRhs;
			if constexpr (Type == PropertyType::Int)			PropertyStatics::BinaryOperators_Int[binaryOperator]->Operate(*lhs, *rhs);
			else if constexpr (Type == PropertyType::Int64)		PropertyStatics::BinaryOperators_Int64[binaryOperator]->Operate(*lhs, *rhs);
			else if constexpr (Type == PropertyType::Float)		PropertyStatics::BinaryOperators_Float[binaryOperator]->Operate(*lhs, *rhs);
			else if constexpr (Type == PropertyType::Double)	PropertyStatics::BinaryOperators_Double[binaryOperator]->Operate(*lhs, *rhs);
			else if constexpr (Type == PropertyType::String)	PropertyStatics::BinaryOperators_String[binaryOperator]->Operate(*lhs, *rhs);
		}
		
	}
};

constexpr PropertyType_t PT_0 = PropertyType::Int;
constexpr PropertyType_t PT_MAX = PropertyType::Unknown;

constexpr PropertyArgumentType_t PAT_0 = PropertyArgumentType::Int8;
constexpr PropertyArgumentType_t PAT_MAX = PropertyArgumentType::Unknown;


template <PropertyType_t I, PropertyArgumentType_t J>
void CreateSelectors() {
	if constexpr (I == PT_MAX) return;
	else {
		if constexpr (J == PAT_MAX) CreateSelectors<static_cast<PropertyType_t>(I + 1), PAT_0>();
		else {
			PropertyStatics::BinaryOperatorSelectors[I][J] = dbg_new PropertyValueBinaryOperatorSelector<I, J>{};
			CreateSelectors<I, static_cast<PropertyArgumentType_t>(J + 1)>();
		}
	}
}


void InitializePropertyOperatorSelectors() {
	CreateSelectors<PT_0, PAT_0>();
}

