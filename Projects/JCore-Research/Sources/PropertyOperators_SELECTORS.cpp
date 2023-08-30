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
		ArgTy* rhs = (ArgTy*)decayedRhs;

		if constexpr (Type == PropertyType::Int)			PropertyStatics::BinaryOperators_Int[binaryOperator]->Operate(*lhs, *rhs);
		else if constexpr (Type == PropertyType::Int64)		PropertyStatics::BinaryOperators_Int64[binaryOperator]->Operate(*lhs, *rhs);
		else if constexpr (Type == PropertyType::Float)		PropertyStatics::BinaryOperators_Float[binaryOperator]->Operate(*lhs, *rhs);
		else if constexpr (Type == PropertyType::Double)	PropertyStatics::BinaryOperators_Double[binaryOperator]->Operate(*lhs, *rhs);
		else if constexpr (Type == PropertyType::String)	PropertyStatics::BinaryOperators_String[binaryOperator]->Operate(*lhs, *rhs);
	}
};

void InitializePropertyOperatorSelectors() {
	// =============================================================================
	// Int
	// =============================================================================
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int64]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int64>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int64U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int64U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int]			= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int32U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int32U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int32L]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int32L>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int32UL]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int32UL>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int16]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int16>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int16U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int16U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int8]			= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int8>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Int8U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Int8U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::WideChar]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::WideChar>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Float]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Float>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::Double]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::Double>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::LDouble]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::LDouble>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::CharPtr]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::CharPtr>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::CharArray]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::CharArray>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int][PropertyArgumentType::String]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int, PropertyArgumentType::String>{};


	// =============================================================================
	// Int64 
	// =============================================================================
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int64]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int64>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int64U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int64U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int32U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int32U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int32L]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int32L>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int32UL]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int32UL>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int16]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int16>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int16U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int16U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int8]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int8>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Int8U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Int8U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::WideChar]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::WideChar>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Float]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Float>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::Double]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::Double>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::LDouble]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::LDouble>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::CharPtr]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::CharPtr>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::CharArray]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::CharArray>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Int64][PropertyArgumentType::String]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Int64, PropertyArgumentType::String>{};

	// =============================================================================
	// Float
	// =============================================================================
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int64]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int64>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int64U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int64U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int32U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int32U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int32L]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int32L>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int32UL]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int32UL>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int16]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int16>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int16U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int16U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int8]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int8>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Int8U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Int8U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::WideChar]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::WideChar>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Float]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Float>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::Double]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::Double>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::LDouble]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::LDouble>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::CharPtr]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::CharPtr>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::CharArray]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::CharArray>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Float][PropertyArgumentType::String]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Float, PropertyArgumentType::String>{};

	// =============================================================================
	// Double
	// =============================================================================
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int64]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int64>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int64U]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int64U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int32U]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int32U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int32L]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int32L>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int32UL]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int32UL>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int16]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int16>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int16U]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int16U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int8]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int8>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Int8U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Int8U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::WideChar]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::WideChar>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Float]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Float>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::Double]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::Double>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::LDouble]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::LDouble>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::CharPtr]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::CharPtr>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::CharArray]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::CharArray>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::Double][PropertyArgumentType::String]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::Double, PropertyArgumentType::String>{};

	// =============================================================================
	// String
	// =============================================================================
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int64]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int64>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int64U]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int64U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int32U]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int32U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int32L]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int32L>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int32UL]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int32UL>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int16]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int16>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int16U]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int16U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int8]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int8>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Int8U]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Int8U>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::WideChar]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::WideChar>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Float]		= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Float>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::Double]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::Double>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::LDouble]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::LDouble>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::CharPtr]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::CharPtr>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::CharArray]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::CharArray>{};
	PropertyStatics::BinaryOperatorSelectors[PropertyType::String][PropertyArgumentType::String]	= dbg_new PropertyValueBinaryOperatorSelector<PropertyType::String, PropertyArgumentType::String>{};
}

