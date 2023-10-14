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

SG_OPERATION_DEF_BEGIN(Int8, Store)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs,	   LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8, Move)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs,	   LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8, Plus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs += rhs,	    LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs += rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs += (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8, Minus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs -= rhs,	    LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs -= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs -= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8, Multiply)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs *= rhs,	    LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs *= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs *= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8, Divide)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs /= rhs,	    LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs /= rhs)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs /= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs /= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs /= (Int8)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(Int8, Modulus)
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

SG_OPERATION_DEF_BEGIN(Int8, Equal)
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

SG_OPERATION_DEF_BEGIN(Int8, NotEqual)
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

SG_OPERATION_DEF_BEGIN(Int8, GreatorEqual)
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


SG_OPERATION_DEF_BEGIN(Int8, Greator)
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

SG_OPERATION_DEF_BEGIN(Int8, LessEqual)
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

SG_OPERATION_DEF_BEGIN(Int8, Less)
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


void InitializePropertyOperatorInt8() {
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Store]		= dbg_new PropertyValueBinaryOperator_Int8_Store{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Move]			= dbg_new PropertyValueBinaryOperator_Int8_Move{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Plus]			= dbg_new PropertyValueBinaryOperator_Int8_Plus{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Minus]		= dbg_new PropertyValueBinaryOperator_Int8_Minus{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Multiply]		= dbg_new PropertyValueBinaryOperator_Int8_Multiply{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Divide]		= dbg_new PropertyValueBinaryOperator_Int8_Divide{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Modulus]		= dbg_new PropertyValueBinaryOperator_Int8_Modulus{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Equal]		= dbg_new PropertyValueBinaryOperator_Int8_Equal{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::NotEqual]		= dbg_new PropertyValueBinaryOperator_Int8_NotEqual{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::GreatorEqual]	= dbg_new PropertyValueBinaryOperator_Int8_GreatorEqual{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Greator]		= dbg_new PropertyValueBinaryOperator_Int8_Greator{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::LessEqual]	= dbg_new PropertyValueBinaryOperator_Int8_LessEqual{};
	PropertyStatics::BinaryOperators_Int8[PropertyBinaryOperatorType::Less]			= dbg_new PropertyValueBinaryOperator_Int8_Less{};

}

NS_JC_END