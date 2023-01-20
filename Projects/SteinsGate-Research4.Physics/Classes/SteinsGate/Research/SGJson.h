/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <json.h>

struct SGAnimationInfo;
class SGJson
{
public:
	static SGString getString(Json::Value& value);

	static void parseAnimationInfo(Json::Value& animationRoot, Out_ SGAnimationInfo& info);
};


