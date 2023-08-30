/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 11:53:57 AM
 * =====================
 *
 */


#pragma once

#include "PropertyOperator.h"
#include "PropertyFactory.h"

struct PropertyStatics
{
	static void Initialize();
	static void Finalize();

	inline static IPropertyValueBinaryOperator<Int8>*		BinaryOperators_Int8		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int8U>*		BinaryOperators_Int8U		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int16>*		BinaryOperators_Int16		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int16U>*		BinaryOperators_Int16U		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int>*		BinaryOperators_Int			[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int32U>*		BinaryOperators_Int32U		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int32L>*		BinaryOperators_Int32L		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int32UL>*	BinaryOperators_Int32UL		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int64>*		BinaryOperators_Int64		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Int64U>*		BinaryOperators_Int64U		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Float>*		BinaryOperators_Float		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<Double>*		BinaryOperators_Double		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<LDouble>*	BinaryOperators_LDouble		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<String>*		BinaryOperators_String		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperatorSelector*		BinaryOperatorSelectors		[PropertyType::Max][PropertyType::Max]{};
	inline static IPropertyValueUnaryOperator*				UnaryOperators				[PropertyType::Max]{};
	inline static IPropertyFactory*							Factorys					[PropertyType::Max]{};


	inline static bool										EnableOperatorLog = true;
	inline static thread_local bool							ComparisonResult{};
};

