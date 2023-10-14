/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 10:25:34 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Container/PropertyOperator.h>
#include <JCore/Container/PropertyOperatorDefinitionMacro.h>
#include <JCore/Container/PropertyStatics.h>

NS_JC_BEGIN

SG_OPERATION_DEF_BEGIN(Int64, Store)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, Move)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, Plus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs += rhs,		 LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs += (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs += (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs += (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, Minus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs -= rhs,		 LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs -= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs -= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs -= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, Multiply)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs *= rhs,		 LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs *= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs *= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs *= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END



SG_OPERATION_DEF_BEGIN(Int64, Divide)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs /= rhs,		 LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs /= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs /= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs /= (Int64)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, Modulus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs %= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs %= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, Equal)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, NotEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, GreatorEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


SG_OPERATION_DEF_BEGIN(Int64, Greator)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, LessEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int64, Less)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


void InitializePropertyOperatorInt64() {
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Store]			= dbg_new PropertyValueBinaryOperator_Int64_Store{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Move]			= dbg_new PropertyValueBinaryOperator_Int64_Move{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Plus]			= dbg_new PropertyValueBinaryOperator_Int64_Plus{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Minus]			= dbg_new PropertyValueBinaryOperator_Int64_Minus{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Multiply]		= dbg_new PropertyValueBinaryOperator_Int64_Multiply{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Divide]			= dbg_new PropertyValueBinaryOperator_Int64_Divide{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Modulus]			= dbg_new PropertyValueBinaryOperator_Int64_Modulus{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Equal]			= dbg_new PropertyValueBinaryOperator_Int64_Equal{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::NotEqual]		= dbg_new PropertyValueBinaryOperator_Int64_NotEqual{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::GreatorEqual]	= dbg_new PropertyValueBinaryOperator_Int64_GreatorEqual{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Greator]			= dbg_new PropertyValueBinaryOperator_Int64_Greator{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::LessEqual]		= dbg_new PropertyValueBinaryOperator_Int64_LessEqual{};
	PropertyStatics::BinaryOperators_Int64[PropertyBinaryOperatorType::Less]			= dbg_new PropertyValueBinaryOperator_Int64_Less{};

}

NS_JC_END