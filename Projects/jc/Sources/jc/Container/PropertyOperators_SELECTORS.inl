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
			else if constexpr (Type == PropertyType::_s8)       PropertyStatics::BinaryOperators__s8[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_u8)      PropertyStatics::BinaryOperators__u8[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_s16)      PropertyStatics::BinaryOperators__s16[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_u16)     PropertyStatics::BinaryOperators__u16[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_s32)        PropertyStatics::BinaryOperators__s32[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_u32)     PropertyStatics::BinaryOperators__u32[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_s32l)     PropertyStatics::BinaryOperators__s32l[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_u32l)    PropertyStatics::BinaryOperators__u32l[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_s64)      PropertyStatics::BinaryOperators__s64[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_u64)     PropertyStatics::BinaryOperators__u64[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_f32)      PropertyStatics::BinaryOperators__f32[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_f64)     PropertyStatics::BinaryOperators__f64[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::_f64l)    PropertyStatics::BinaryOperators__f64l[_binaryOperator]->Operate(*pLhs, pRhs);
			else if constexpr (Type == PropertyType::String)     PropertyStatics::BinaryOperators_String[_binaryOperator]->Operate(*pLhs, pRhs);
		}
		else
		{
			ArgTy* pRhs = (ArgTy*)_pDecayedRhs;
			if constexpr (Type == PropertyType::Bool)            PropertyStatics::BinaryOperators_bool[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_s8)       PropertyStatics::BinaryOperators__s8[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_u8)      PropertyStatics::BinaryOperators__u8[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_s16)      PropertyStatics::BinaryOperators__s16[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_u16)     PropertyStatics::BinaryOperators__u16[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_s32)        PropertyStatics::BinaryOperators__s32[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_u32)     PropertyStatics::BinaryOperators__u32[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_s32l)     PropertyStatics::BinaryOperators__s32l[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_u32l)    PropertyStatics::BinaryOperators__u32l[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_s64)      PropertyStatics::BinaryOperators__s64[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_u64)     PropertyStatics::BinaryOperators__u64[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_f32)      PropertyStatics::BinaryOperators__f32[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_f64)     PropertyStatics::BinaryOperators__f64[_binaryOperator]->Operate(*pLhs, *pRhs);
			else if constexpr (Type == PropertyType::_f64l)    PropertyStatics::BinaryOperators__f64l[_binaryOperator]->Operate(*pLhs, *pRhs);
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