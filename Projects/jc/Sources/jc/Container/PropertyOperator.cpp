/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:04:02 PM
 * =====================
 *
 */

#include <jc/Core.h>
#include <jc/Container/PropertyOperator.h>
#include <jc/Container/Property.h>

NS_JC_BEGIN
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogLossDataPossible(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType)
{
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산을 수행하였습니다. (데이터가 잘릴 위험이 있습니다.)", PropertyType::NAME[_lhs],
	          PropertyBinaryOperatorType::NAME[_operatorType], PropertyType::NAME[_rhs]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogImpossibleOperation(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType)
{
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogError_("%s %s %s연산은 수행이 불가능합니다.", PropertyType::NAME[_lhs], PropertyBinaryOperatorType::NAME[_operatorType],
	           PropertyType::NAME[_rhs]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogUnsafeOperation(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType)
{
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산은 안전하지 않습니다.", PropertyType::NAME[_lhs], PropertyBinaryOperatorType::NAME[_operatorType],
	          PropertyType::NAME[_rhs]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogUnknownOperation(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType)
{
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산을 수행하였습니다. (데이터가 잘릴 위험이 있습니다.)", PropertyType::NAME[_lhs],
	          PropertyBinaryOperatorType::NAME[_operatorType], PropertyType::NAME[_rhs]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void LogOperation(PropertyType_t _lhs, PropertyType_t _rhs, PropertyBinaryOperatorType_t _operatorType)
{
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산을 수행하였습니다.", PropertyType::NAME[_lhs], PropertyBinaryOperatorType::NAME[_operatorType],
	          PropertyType::NAME[_rhs]);
}

NS_END
