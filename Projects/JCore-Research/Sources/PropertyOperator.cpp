/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:04:02 PM
 * =====================
 *
 */

#include "PropertyOperator.h"
#include "Property.h"

void LogLossDataPossible(PropertyArgumentType_t lhs, PropertyArgumentType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산을 수행하였습니다. (데이터가 잘릴 위험이 있습니다.)", PropertyArgumentType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyArgumentType::Name[rhs]);
}

void LogImpossibleOperation(PropertyArgumentType_t lhs, PropertyArgumentType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산은 수행이 불가능합니다.", PropertyArgumentType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyArgumentType::Name[rhs]);
}
void LogUnsafeOperation(PropertyArgumentType_t lhs, PropertyArgumentType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산은 안전하지 않습니다.", PropertyArgumentType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyArgumentType::Name[rhs]);
}

void LogUnknownOperation(PropertyArgumentType_t lhs, PropertyArgumentType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산을 수행하였습니다. (데이터가 잘릴 위험이 있습니다.)", PropertyArgumentType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyArgumentType::Name[rhs]);
}
