/*
 * 작성자: 윤정도
 * 생성일: 8/30/2023 11:53:57 AM
 * =====================
 *
 */


#pragma once

#include <jc/Container/PropertyOperator.h>
#include <jc/Container/PropertyFactory.h>

NS_JC_BEGIN

struct PropertyStatics
{
	static void Initialize();
	static void Finalize();

	inline static IPropertyValueBinaryOperator<bool>*		BinaryOperators_bool		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_s8>*		BinaryOperators__s8		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_u8>*		BinaryOperators__u8		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_s16>*		BinaryOperators__s16		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_u16>*		BinaryOperators__u16		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_s32>*		BinaryOperators__s32		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_u32>*		BinaryOperators__u32		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_s32l>*		BinaryOperators__s32l		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_u32l>*		BinaryOperators__u32l		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_s64>*		BinaryOperators__s64		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_u64>*		BinaryOperators__u64		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_f32>*		BinaryOperators__f32		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_f64>*		BinaryOperators__f64		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<_f64l>*		BinaryOperators__f64l		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperator<String>*		BinaryOperators_String		[PropertyBinaryOperatorType::Max]{};
	inline static IPropertyValueBinaryOperatorSelector*		BinaryOperatorSelectors		[PropertyType::Max][PropertyType::Max]{};
	inline static IPropertyValueUnaryOperator*				UnaryOperators				[PropertyType::Max]{};
	inline static IPropertyFactory*							Factorys					[PropertyType::Max]{};
	inline static PropertyBase*								DefaultPrperty				[PropertyType::Max]{};

	inline static bool										EnableOperatorLog = true;
	inline static thread_local bool							ComparisonResult{};
};

NS_END