/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Core.h"
#include "SGJson.h"

#include <SteinsGate/Common/SGTextParser.h>

SGString SGJson::getString(Json::Value& value) {
	DebugAssertMsg(!value.isNull(), "값이 없습니다.");
	const char* pBegin;
	const char* pEnd;
	value.getString(&pBegin, &pEnd);
	const int iLen = pEnd - pBegin;
	return { pBegin, iLen + 1 };
}

void SGJson::parseThicknessInfo(Json::Value& thicknessRoot, ThicknessBox& info) {

	int num[4];
	SGTextParser::parserIntNumbers(SGJson::getString(thicknessRoot), num, 4);

	info.RelativeX = (float)num[0];
	info.RelativeY = (float)num[1];
	info.Width = (float)num[2];
	info.Height = (float)num[3];
}

void SGJson::parseIntNumber3(Json::Value& root, int& num1, int& num2, int& num3) {
	int num[3];
	SGTextParser::parserIntNumbers(SGJson::getString(root), num, 3);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
}

void SGJson::parseIntNumber4(Json::Value& root, int& num1, int& num2, int& num3, int& num4) {

	int num[4];
	SGTextParser::parserIntNumbers(SGJson::getString(root), num, 4);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
	num4 = num[3];
}

void SGJson::parseIntNumberN(Json::Value& root, int* numArr, int count) {
	SGTextParser::parserIntNumbers(SGJson::getString(root), numArr, count);
}

void SGJson::parseFloatNumber2(Json::Value& root, float& num1, float& num2) {
	float num[2];
	SGTextParser::parserFloatNumbers(SGJson::getString(root), num, 2);
	num1 = num[0];
	num2 = num[1];
}

void SGJson::parseFloatNumberN(Json::Value& root, float* numArr, int count) {
	SGTextParser::parserFloatNumbers(SGJson::getString(root), numArr, count);
}

SGString SGJson::getStringDefault(Json::Value& value, const SGString& defaultValue) {
	if (value.empty())
		return defaultValue;

	return getString(value);
}

float SGJson::getFloatDefault(Json::Value& value, float defaultValue) {
	if (value.empty())
		return defaultValue;

	return value.asFloat();
}

int SGJson::getIntDefault(Json::Value& value, int defaultValue) {
	if (value.empty())
		return defaultValue;

	return value.asInt();
}