/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>

 //////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AttackDamageType)
None,
Physics,
Magic,
Fixed,
Max
JC_SENUM_END(AttackDamageType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(AttackXForceDirection)
None,
Forward,
Backward,
Spread,
Max
JC_SENUM_END(AttackXForceDirection)


//////////////////////////////////////////////////////////////////////////////////////////
struct AttackDataInfo : SDescBase
{
	~AttackDataInfo() override = default;

	jc::String name_;
	AttackDamageType_t attackDamageType_;
	float attackDamageRatio_;
	AttackXForceDirection_t attackXForceDir_;
	float attackXForce_;
	float attackYForce_;
	bool isFallDownAttack_;
};
