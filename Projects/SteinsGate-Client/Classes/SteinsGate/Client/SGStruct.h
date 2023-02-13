/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Common/SGEnums.h>
#include <SteinsGate/Common/SGStruct.h>

struct SGAnimationInfo;
struct SGAttackDataInfo;

class SGActor;
class SGPhysicsActor;



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

	SGVector<SGActorPartSpriteData> Parts;
	SGVector<SGAnimationInfo*> Animations;
};

using SGActorSpriteDataPtr = JCore::SharedPtr<SGActorSpriteData>;

struct SGHitInfo
{
	SGActor* Attacker;
	SGPhysicsActor* HitTarget;
	SpriteDirection_t HitDirection;
	SGRect HitRect;
	SGAttackDataInfo* AttackDataInfo;
};