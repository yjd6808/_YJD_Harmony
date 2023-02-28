﻿/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Core.h"
#include "SGJson.h"

#include <SteinsGate/Common/SGTextParser.h>

SGString SGJson::getString(Json::Value& value) {
	DebugAssertMsg(!value.isNull(), "인자로 전달한 Json 오브젝트에 문자열 데이터가 없습니다.");
	const char* pBegin;
	const char* pEnd;
	value.getString(&pBegin, &pEnd);
	const int iLen = pEnd - pBegin;
	return { pBegin, iLen + 1 };
}

SGString SGJson::getStringOrDefault(Json::Value& value, const SGString& defaultValue) {
	if (value.empty())
		return defaultValue;

	return getString(value);
}

SGString SGJson::getStringOrNull(Json::Value& value)
{
	if (value.empty())
		return { 0 };

	return getString(value);
}

void SGJson::parseThicknessInfo(Json::Value& thicknessRoot, Out_ ThicknessBox& info) {

	int num[4];
	SGTextParser::parserIntNumbers(SGJson::getString(thicknessRoot), num, 4);

	info.RelativeX = (float)num[0];
	info.RelativeY = (float)num[1];
	info.Width = (float)num[2];
	info.Height = (float)num[3];
}

void SGJson::parseIntNumber3(Json::Value& root, Out_ int& num1, Out_ int& num2, Out_ int& num3) {
	int num[3];
	SGTextParser::parserIntNumbers(SGJson::getString(root), num, 3);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
}

void SGJson::parseIntNumber4(Json::Value& root, Out_ int& num1, Out_ int& num2, Out_ int& num3, Out_ int& num4) {

	int num[4];
	SGTextParser::parserIntNumbers(SGJson::getString(root), num, 4);

	num1 = num[0];
	num2 = num[1];
	num3 = num[2];
	num4 = num[3];
}

void SGJson::parseIntNumberN(Json::Value& root, Out_ int* numArr, int count) {
	SGTextParser::parserIntNumbers(SGJson::getString(root), numArr, count);
}

void SGJson::parseFloatNumber2(Json::Value& root, Out_ float& num1, Out_ float& num2) {
	float num[2];
	SGTextParser::parserFloatNumbers(SGJson::getString(root), num, 2);
	num1 = num[0];
	num2 = num[1];
}

void SGJson::parseFloatNumberN(Json::Value& root, Out_ float* numArr, int count) {
	SGTextParser::parserFloatNumbers(SGJson::getString(root), numArr, count);
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