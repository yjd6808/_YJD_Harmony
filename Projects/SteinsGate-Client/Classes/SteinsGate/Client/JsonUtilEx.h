/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Common/JsonUtil.h>

struct AnimationInfo;

struct JsonUtilEx : JsonUtil
{
public:
	static void ParseAnimationInfo(Json::Value& _animationRoot, OUT AnimationInfo& _info);
	static void ParseActorRect(Json::Value& _root, OUT ActorRect& _actorRect);
	static void ParseActorSpriteData(Json::Value& _actorSpriteDataRoot, OUT ActorSpriteData** _ppInfo);
	static void ParseColor4B(Json::Value& _root, OUT SGColor4B& _color);
	static void ParseSize(Json::Value& _root, OUT SGSize& _size);
	static void ParseVec2(Json::Value& _root, OUT SGVec2& _vec);
};
