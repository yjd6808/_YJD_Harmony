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

SG_OPERATION_DEF_BEGIN(Int8U, Store)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8U, Move)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8U, Plus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs += rhs,		 LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs += (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8U, Minus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs -= rhs,		 LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs -= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8U, Multiply)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs *= rhs,		 LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs *= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8U, Divide)
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
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs /= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs /= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs /= (Int8U)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8U, Modulus)
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

SG_OPERATION_DEF_BEGIN(Int8U, Equal)
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

SG_OPERATION_DEF_BEGIN(Int8U, NotEqual)
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

SG_OPERATION_DEF_BEGIN(Int8U, GreatorEqual)
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


SG_OPERATION_DEF_BEGIN(Int8U, Greator)
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

SG_OPERATION_DEF_BEGIN(Int8U, LessEqual)
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

SG_OPERATION_DEF_BEGIN(Int8U, Less)
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


void InitializePropertyOperatorInt8U() {
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Store]			= dbg_new PropertyValueBinaryOperator_Int8U_Store{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Move]			= dbg_new PropertyValueBinaryOperator_Int8U_Move{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Plus]			= dbg_new PropertyValueBinaryOperator_Int8U_Plus{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Minus]			= dbg_new PropertyValueBinaryOperator_Int8U_Minus{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Multiply]		= dbg_new PropertyValueBinaryOperator_Int8U_Multiply{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Divide]			= dbg_new PropertyValueBinaryOperator_Int8U_Divide{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Modulus]			= dbg_new PropertyValueBinaryOperator_Int8U_Modulus{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Equal]			= dbg_new PropertyValueBinaryOperator_Int8U_Equal{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::NotEqual]		= dbg_new PropertyValueBinaryOperator_Int8U_NotEqual{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::GreatorEqual]	= dbg_new PropertyValueBinaryOperator_Int8U_GreatorEqual{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Greator]			= dbg_new PropertyValueBinaryOperator_Int8U_Greator{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::LessEqual]		= dbg_new PropertyValueBinaryOperator_Int8U_LessEqual{};
	PropertyStatics::BinaryOperators_Int8U[PropertyBinaryOperatorType::Less]			= dbg_new PropertyValueBinaryOperator_Int8U_Less{};

}

NS_JC_END