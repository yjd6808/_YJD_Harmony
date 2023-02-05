/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Research/SGStruct.h>
#include <json.h>

struct SGAnimationInfo;
class SGJson
{
public:
	static SGString getString(Json::Value& value);

	static void parseAnimationInfo(Json::Value& animationRoot, Out_ SGAnimationInfo& info);
	static void parseThicknessInfo(Json::Value& thicknessRoot, Out_ SGThicknessBox& box);

	static void parseIntNumber3(Json::Value& root, Out_ int& num1, Out_ int& num2, Out_ int& num3);
	static void parseIntNumber4(Json::Value& root, Out_ int& num1, Out_ int& num2, Out_ int& num3, Out_ int& num4);
	static void parseIntNumberN(Json::Value& root, Out_ int* numArr, int count /* */);

	static void parseFloatNumber2(Json::Value& root, Out_ float& num1, Out_ float& num2);
	static void parseFloatNumberN(Json::Value& root, Out_ float* numArr, float count /* */);

	static int getIntDefault(Json::Value& value, int defaultValue = 0);
	static float getFloatDefault(Json::Value& value, float defaultValue = 0);
	static SGString getStringDefault(Json::Value& value, const SGString& defaultValue = 0); // 내 문자열은 0으로 초기화하면 동적 할당 안함
};




