/*
 * 작성자: 윤정도
 * 생성일: 8/29/2023 6:04:02 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Container/PropertyOperator.h>
#include <JCore/Container/Property.h>

NS_JC_BEGIN

void LogLossDataPossible(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산을 수행하였습니다. (데이터가 잘릴 위험이 있습니다.)", PropertyType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyType::Name[rhs]);
}

void LogImpossibleOperation(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogError_("%s %s %s연산은 수행이 불가능합니다.", PropertyType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyType::Name[rhs]);
}
void LogUnsafeOperation(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산은 안전하지 않습니다.", PropertyType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyType::Name[rhs]);
}

void LogUnknownOperation(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산을 수행하였습니다. (데이터가 잘릴 위험이 있습니다.)", PropertyType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyType::Name[rhs]);
}

void LogOperation(PropertyType_t lhs, PropertyType_t rhs, PropertyBinaryOperatorType_t operatorType) {
	if (!PropertyStatics::EnableOperatorLog) return;
	_LogWarn_("%s %s %s연산을 수행하였습니다.", PropertyType::Name[lhs], PropertyBinaryOperatorType::Name[operatorType], PropertyType::Name[rhs]);
}

NS_JC_END