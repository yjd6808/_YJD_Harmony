/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGStruct.h>
#include <json.h>

struct SGAnimationInfo;
class SGJson
{
public:
	static SGString getString(Json::Value& value);

	static void parseAnimationInfo(Json::Value& animationRoot, Out_ SGAnimationInfo& info);
	static void parseThicknessInfo(Json::Value& thicknessRoot, Out_ SGThicknessBox& box);

	static void parseNumber3(Json::Value& root, Out_ int& num1, Out_ int& num2, Out_ int& num3);
	static void parseNumber4(Json::Value& root, Out_ int& num1, Out_ int& num2, Out_ int& num3, Out_ int& num4);
	static void parseNumberN(Json::Value& root, Out_ int* numArr, int capacity);
};


