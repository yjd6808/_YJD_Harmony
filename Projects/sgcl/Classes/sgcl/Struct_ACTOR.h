/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 1:25:25 PM
 * =====================
 *
 */


#pragma once

struct ActorRect
{
	c2d::rect thicknessRect_;
	c2d::rect bodyRect_;

	bool IsCollide(const ActorRect& _rect, SpriteDirection_t& _otherHitDirection, c2d::rect& _hitRect);
};

struct HitInfo
{
	Actor* pAttacker_;
	Actor* pHitTarget_;
	SpriteDirection_t hitDirection_;
	c2d::rect hitRect_;
	AttackDataInfo* pAttackDataInfo_;
};
