/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 12:01:19 PM
 * =====================
 *
 */

#include "PropertyStatics.h"
#include "PropertyOperatorDefinitionMacro.h"

NS_JC_BEGIN

// Argument를 Type으로 저장하도록함.
template <PropertyType_t Type, PropertyType_t ArgumentType>
struct SPropertyValueBinaryOperatorSelector : IPropertyValueBinaryOperatorSelector
{
	using Ty = typename PropertyDataTypeGetter<Type>::Ty;
	using ArgTy = typename PropertyDataTypeGetter<ArgumentType>::Ty;

	// 올바른 타입정보를 얻지 못한 경우
	static_assert(!IsVoidType_v<Ty>, "... ty must not be void type");
	static_assert(!IsVoidType_v<ArgTy>, "... argty must not be void type");

	//////////////////////////////////////////////////////////////////////////////////////
	void Select(int* _pDecayedLhs, int* _pDecayedRhs, PropertyBinaryOperatorType_t _binaryOperator) override
	{
		Ty* pLhs = (Ty*)_pDecayedLhs;

		if constexpr (ArgumentType == PropertyType::CharPtr)
		{
			ArgTy pRhs = (ArgTy)_pDecayedRhs;

			if constexpr (Type == PropertyType::Bool)            PropertyStatics::BinaryOperators_bool[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int8)       PropertyStatics::BinaryOperators_Int8[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int8U)      PropertyStatics::BinaryOperators_Int8U[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int16)      PropertyStatics::BinaryOperators_Int16[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int16U)     PropertyStatics::BinaryOperators_Int16U[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int)        PropertyStatics::BinaryOperators_Int[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int32U)     PropertyStatics::BinaryOperators_Int32U[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int32L)     PropertyStatics::BinaryOperators_Int32L[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int32UL)    PropertyStatics::BinaryOperators_Int32UL[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int64)      PropertyStatics::BinaryOperators_Int64[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Int64U)     PropertyStatics::BinaryOperators_Int64U[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Float)      PropertyStatics::BinaryOperators_Float[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::Double)     PropertyStatics::BinaryOperators_Double[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::LDouble)    PropertyStatics::BinaryOperators_LDouble[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::String)     PropertyStatics::BinaryOperators_String[_binaryOperator]->Operate(*pLhs, pRhs);
		}
		else
		{
			ArgTy* pRhs = (ArgTy*)_pDecayedRhs;
			if constexpr (Type == PropertyType::Bool)            PropertyStatics::BinaryOperators_bool[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int8)       PropertyStatics::BinaryOperators_Int8[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int8U)      PropertyStatics::BinaryOperators_Int8U[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int16)      PropertyStatics::BinaryOperators_Int16[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int16U)     PropertyStatics::BinaryOperators_Int16U[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int)        PropertyStatics::BinaryOperators_Int[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int32U)     PropertyStatics::BinaryOperators_Int32U[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int32L)     PropertyStatics::BinaryOperators_Int32L[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int32UL)    PropertyStatics::BinaryOperators_Int32UL[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int64)      PropertyStatics::BinaryOperators_Int64[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Int64U)     PropertyStatics::BinaryOperators_Int64U[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Float)      PropertyStatics::BinaryOperators_Float[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::Double)     PropertyStatics::BinaryOperators_Double[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::LDouble)    PropertyStatics::BinaryOperators_LDouble[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::String)     PropertyStatics::BinaryOperators_String[_binaryOperator]->Operate(*pLhs, *pRhs);
		}
	}
};


//////////////////////////////////////////////////////////////////////////////////////////
template <PropertyType_t I, PropertyType_t J>
void CreateSelectors()
{
	if constexpr (I == PropertyType::Unknown)
		return;
	else
	{
		if constexpr (J == PropertyType::Unknown)
			CreateSelectors<static_cast<PropertyType_t>(I + 1), PropertyType_t(0)>();
		else
		{
			PropertyStatics::BinaryOperatorSelectors[I][J] = dbg_new SPropertyValueBinaryOperatorSelector<I, J>{};
			CreateSelectors<I, static_cast<PropertyType_t>(J + 1)>();
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
void InitializePropertyOperatorSelectors()
{
	CreateSelectors<PropertyType_t(0), PropertyType_t(0)>();
}

NS_END