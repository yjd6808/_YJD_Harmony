/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 1:25:25 PM
 * =====================
 *
 */


#pragma once

struct ActorRect
{
	SGRect ThicknessRect;
	SGRect BodyRect;

	bool isCollide(const ActorRect& _rect, SpriteDirection_t& _otherHitDirection, SGRect& _hitRect);
};

struct HitInfo
{
	Actor* Attacker;
	Actor* HitTarget;
	SpriteDirection_t HitDirection;
	SGRect HitRect;
	AttackDataInfo* AttackDataInfo;
};
