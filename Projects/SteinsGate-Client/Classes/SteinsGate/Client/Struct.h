/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Enum.h>
#include <SteinsGate/Common/Struct.h>

struct AnimationInfo;
struct AttackDataInfo;

class Actor;
class PhysicsActor;



struct SGActorRect
{
	SGRect ThicknessRect;
	SGRect BodyRect;

	bool isCollide(const SGActorRect& rect, SpriteDirection_t& otherHitDirection, SGRect& hitRect);
};


struct SGActorSpriteData
{
	SGActorSpriteData(int partCount, int animationCount)
		: Parts(partCount)
		, Animations(animationCount) {}

	SGVector<ActorPartSpriteData> Parts;
	SGVector<AnimationInfo*> Animations;
};

using SGActorSpriteDataPtr = JCore::SharedPtr<SGActorSpriteData>;

struct SGHitInfo
{
	Actor* Attacker;
	PhysicsActor* HitTarget;
	SpriteDirection_t HitDirection;
	SGRect HitRect;
	AttackDataInfo* AttackDataInfo;
};

