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

SG_OPERATION_DEF_BEGIN(bool, Store)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, Move)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs = rhs)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs = (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, Plus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs += rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs += (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, Minus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs -= rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs -= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, Multiply)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs *= rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs *= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, Divide)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs /= rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(lhs /= (bool)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(lhs /= (bool)rhs, LogLossDataPossible)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, Modulus)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(lhs %= rhs,		LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(lhs %= (bool)rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, Equal)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs == rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, NotEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs != rhs, LogUnsafeOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, GreatorEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs >= rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


SG_OPERATION_DEF_BEGIN(bool, Greator)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs > rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, LessEqual)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs <= rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END

SG_OPERATION_DEF_BEGIN(bool, Less)
SG_OPERATION_OPERATE(const Int64&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int64U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32L&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int32UL&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int16U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const bool&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Int8U&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Float&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const Double&)		SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const LDouble&)	SG_OPERATION_BODY(PropertyStatics::ComparisonResult = lhs < rhs, LogOperation)
SG_OPERATION_OPERATE(const String&)		SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_OPERATE(char*)				SG_OPERATION_BODY(, LogImpossibleOperation)
SG_OPERATION_DEF_END


void InitializePropertyOperatorBool() {
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Store]		= dbg_new PropertyValueBinaryOperator_bool_Store{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Move]			= dbg_new PropertyValueBinaryOperator_bool_Move{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Plus]			= dbg_new PropertyValueBinaryOperator_bool_Plus{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Minus]		= dbg_new PropertyValueBinaryOperator_bool_Minus{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Multiply]		= dbg_new PropertyValueBinaryOperator_bool_Multiply{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Divide]		= dbg_new PropertyValueBinaryOperator_bool_Divide{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Modulus]		= dbg_new PropertyValueBinaryOperator_bool_Modulus{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Equal]		= dbg_new PropertyValueBinaryOperator_bool_Equal{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::NotEqual]		= dbg_new PropertyValueBinaryOperator_bool_NotEqual{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::GreatorEqual]	= dbg_new PropertyValueBinaryOperator_bool_GreatorEqual{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Greator]		= dbg_new PropertyValueBinaryOperator_bool_Greator{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::LessEqual]	= dbg_new PropertyValueBinaryOperator_bool_LessEqual{};
	PropertyStatics::BinaryOperators_bool[PropertyBinaryOperatorType::Less]			= dbg_new PropertyValueBinaryOperator_bool_Less{};

}

NS_JC_END