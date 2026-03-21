/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include "sgcl/JsonUtilEx.h"

struct AnimationInfo;

struct JsonUtilEx : JsonUtil
{
public:
	static void ParseActorSpriteData(Json::Value& _actorSpriteDataRoot, OUT ActorSpriteData** _ppInfo);
};
