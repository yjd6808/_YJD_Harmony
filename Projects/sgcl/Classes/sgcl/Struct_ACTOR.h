/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 1:25:25 PM
 * =====================
 *
 */


#pragma once


struct HitInfo
{
	Actor* pAttacker_;
	Actor* pHitTarget_;
	SpriteDirection_t hitDirection_;
	cc::rect hitRect_;
	AttackDataInfo* pAttackDataInfo_;
};
