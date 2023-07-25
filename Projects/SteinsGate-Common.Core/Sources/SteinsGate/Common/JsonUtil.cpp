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

const char* JsonUtil::getStringRaw(Json::Value& value, JCORE_OUT_OPT int* len /* = nullptr */) {
	DebugAssertMsg(!value.isNull(), "인자로 전달한 Json 오브젝트에 문자열 데이터가 없습니다.");
	const char* pBegin;
	const char* pEnd;
	value.getString(&pBegin, &pEnd);
	if (len != nullptr)
		*len = pEnd - pBegin;
	return pBegin;
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

void JsonUtil::parseThicknessInfo(Json::Value& thicknessRoot, JCORE_OUT ThicknessBox& info) {

	int num[4];
	TextParser::parseIntNumbers(getString(thicknessRoot), num, 4);

	info.RelativeX = (float)num[0];
	info.RelativeY = (float)num[1];
	info.Width = (float)num[2];
	info.Height = (float)num[3];
}

void JsonUtil::parseIntNumber3(Json::Value& root, JCORE_OUT int& num1, JCORE_OUT int& num2, JCORE_OUT int& num3) {
	int num[3];
	TextParser::parseIntNumbers(getString(root), num, 3);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
}

void JsonUtil::parseIntNumber4(Json::Value& root, JCORE_OUT int& num1, JCORE_OUT int& num2, JCORE_OUT int& num3, JCORE_OUT int& num4) {

	int num[4];
	TextParser::parseIntNumbers(getString(root), num, 4);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
	num4 = num[3];
}

void JsonUtil::parseIntNumberN(Json::Value& root, JCORE_OUT int* numArr, int count) {
	TextParser::parseIntNumbers(getString(root), numArr, count);
}

void JsonUtil::parseFloatNumber2(Json::Value& root, JCORE_OUT float& num1, JCORE_OUT float& num2) {
	float num[2];
	TextParser::parseFloatNumbers(getString(root), num, 2);
	num1 = num[0];
	num2 = num[1];
}

void JsonUtil::parseFloatNumberN(Json::Value& root, JCORE_OUT float* numArr, int count) {
	TextParser::parseFloatNumbers(getString(root), numArr, count);
}



float JsonUtil::getFloatDefault(Json::Value& value, float defaultValue /* = 0 */) {
	if (value.empty())
		return defaultValue;

	return value.asFloat();
}

int JsonUtil::getIntDefault(Json::Value& value, int defaultValue /* = 0 */) {
	if (value.empty())
		return defaultValue;

	return value.asInt();
}

bool JsonUtil::getBooleanDefault(Json::Value& value, bool defaultValue /* = false */) {
	if (value.empty())
		return defaultValue;

	return value.asBool();
}
