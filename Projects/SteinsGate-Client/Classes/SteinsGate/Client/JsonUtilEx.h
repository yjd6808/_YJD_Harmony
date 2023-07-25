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
	static void parseAnimationInfo(Json::Value& animationRoot, JCORE_OUT AnimationInfo& info);
	static void parseActorRect(Json::Value& root, JCORE_OUT ActorRect& actorRect);
	static void parseActorSpriteData(Json::Value& actorSpriteDataRoot, JCORE_OUT ActorSpriteData** info);
	static void parseColor4B(Json::Value& root, JCORE_OUT SGColor4B& color);
	static void parseSize(Json::Value& root, JCORE_OUT SGSize& size);
	static void parseVec2(Json::Value& root, JCORE_OUT SGVec2& vec);
};




