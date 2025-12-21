/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 1:25:25 PM
 * =====================
 *
 */


#pragma once

struct ActorRect
{
	SGRect thicknessRect_;
	SGRect bodyRect_;

	bool IsCollide(const ActorRect& _rect, SpriteDirection_t& _otherHitDirection, SGRect& _hitRect);
};

struct HitInfo
{
	Actor* pAttacker_;
	Actor* pHitTarget_;
	SpriteDirection_t hitDirection_;
	SGRect hitRect_;
	AttackDataInfo* pAttackDataInfo_;
};
