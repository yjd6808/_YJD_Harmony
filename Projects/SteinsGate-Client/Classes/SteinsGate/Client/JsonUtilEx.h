/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Common/JsonUtil.h>

struct AnimationInfo;
struct JsonUtilEx : JsonUtil
{
public:
	static void parseAnimationInfo(Json::Value& animationRoot, Out_ AnimationInfo& info);
	static void parseColor4B(Json::Value& root, Out_ SGColor4B& color);
	static void parseSize(Json::Value& root, Out_ SGSize& size);
	static void parseVec2(Json::Value& root, Out_ SGVec2& vec);
};




