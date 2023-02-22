/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Common/SGJson.h>

struct SGAnimationInfo;
struct SGJsonEx : SGJson
{
public:
	static void parseAnimationInfo(Json::Value& animationRoot, Out_ SGAnimationInfo& info);
	static void parseColor4B(Json::Value& root, Out_ SGColor4B& color);
};




