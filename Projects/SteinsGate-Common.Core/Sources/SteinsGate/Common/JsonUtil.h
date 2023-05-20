/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include "Core.h"

struct JsonUtil
{
public:
	static SGString getString(Json::Value& value);
	static SGString getStringOrNull(Json::Value& value);
	static SGString getStringOrDefault(Json::Value& value, const SGString& defaultValue = 0); // 내 문자열은 0으로 초기화하면 동적 할당 안함
	static void parseThicknessInfo(Json::Value& thicknessRoot, JCORE_OUT ThicknessBox& box);

	static void parseIntNumber3(Json::Value& root, JCORE_OUT int& num1, JCORE_OUT int& num2, JCORE_OUT int& num3);
	static void parseIntNumber4(Json::Value& root, JCORE_OUT int& num1, JCORE_OUT int& num2, JCORE_OUT int& num3, JCORE_OUT int& num4);
	static void parseIntNumberN(Json::Value& root, JCORE_OUT int* numArr, int count);

	static void parseFloatNumber2(Json::Value& root, JCORE_OUT float& num1, JCORE_OUT float& num2);
	static void parseFloatNumberN(Json::Value& root, JCORE_OUT float* numArr, int count);

	static int getIntDefault(Json::Value& value, int defaultValue = 0);
	static float getFloatDefault(Json::Value& value, float defaultValue = 0);
	
};




