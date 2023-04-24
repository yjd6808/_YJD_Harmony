/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Core.h"
#include "JsonUtil.h"

#include <SteinsGate/Common/TextParser.h>

SGString JsonUtil::getString(Json::Value& value) {
	DebugAssertMsg(!value.isNull(), "인자로 전달한 Json 오브젝트에 문자열 데이터가 없습니다.");
	const char* pBegin;
	const char* pEnd;
	value.getString(&pBegin, &pEnd);
	const int iLen = pEnd - pBegin;
	return { pBegin, iLen + 1 };
}

SGString JsonUtil::getStringOrDefault(Json::Value& value, const SGString& defaultValue) {
	if (value.empty())
		return defaultValue;

	return getString(value);
}

SGString JsonUtil::getStringOrNull(Json::Value& value)
{
	if (value.empty())
		return { 0 };

	return getString(value);
}

void JsonUtil::parseThicknessInfo(Json::Value& thicknessRoot, Out_ ThicknessBox& info) {

	int num[4];
	TextParser::parserIntNumbers(getString(thicknessRoot), num, 4);

	info.RelativeX = (float)num[0];
	info.RelativeY = (float)num[1];
	info.Width = (float)num[2];
	info.Height = (float)num[3];
}

void JsonUtil::parseIntNumber3(Json::Value& root, Out_ int& num1, Out_ int& num2, Out_ int& num3) {
	int num[3];
	TextParser::parserIntNumbers(getString(root), num, 3);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
}

void JsonUtil::parseIntNumber4(Json::Value& root, Out_ int& num1, Out_ int& num2, Out_ int& num3, Out_ int& num4) {

	int num[4];
	TextParser::parserIntNumbers(getString(root), num, 4);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
	num4 = num[3];
}

void JsonUtil::parseIntNumberN(Json::Value& root, Out_ int* numArr, int count) {
	TextParser::parserIntNumbers(getString(root), numArr, count);
}

void JsonUtil::parseFloatNumber2(Json::Value& root, Out_ float& num1, Out_ float& num2) {
	float num[2];
	TextParser::parserFloatNumbers(getString(root), num, 2);
	num1 = num[0];
	num2 = num[1];
}

void JsonUtil::parseFloatNumberN(Json::Value& root, Out_ float* numArr, int count) {
	TextParser::parserFloatNumbers(getString(root), numArr, count);
}



float JsonUtil::getFloatDefault(Json::Value& value, float defaultValue) {
	if (value.empty())
		return defaultValue;

	return value.asFloat();
}

int JsonUtil::getIntDefault(Json::Value& value, int defaultValue) {
	if (value.empty())
		return defaultValue;

	return value.asInt();
}