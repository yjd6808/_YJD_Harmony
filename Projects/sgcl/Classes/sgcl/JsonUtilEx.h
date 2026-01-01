/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

#include <sgcl/Core.h>
#include <sg/JsonUtil.h>

struct AnimationInfo;

struct JsonUtilEx : JsonUtil
{
public:
	static void ParseAnimationInfo(Json::Value& _animationRoot, OUT AnimationInfo& _info);
	static void ParseActorRect(Json::Value& _root, OUT ActorRect& _actorRect);
	static void ParseActorSpriteData(Json::Value& _actorSpriteDataRoot, OUT ActorSpriteData** _ppInfo);
	static void ParseColor4B(Json::Value& _root, OUT c2d::Color4B& _color);
	static void ParseSize(Json::Value& _root, OUT c2d::size& _size);
	static void ParseVec2(Json::Value& _root, OUT c2d::vec2& _vec);
};
