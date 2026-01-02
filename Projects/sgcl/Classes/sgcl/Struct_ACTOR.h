/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 1:25:25 PM
 * =====================
 *
 */


#pragma once

struct ActorRect
{
	cc::rect thicknessRect_;
	cc::rect bodyRect_;

	bool IsCollide(const ActorRect& _rect, SpriteDirection_t& _otherHitDirection, cc::rect& _hitRect);
};

struct HitInfo
{
	Actor* pAttacker_;
	Actor* pHitTarget_;
	SpriteDirection_t hitDirection_;
	cc::rect hitRect_;
	AttackDataInfo* pAttackDataInfo_;
};
