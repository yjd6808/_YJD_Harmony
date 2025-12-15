/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct AttackDataInfo : ConfigDataAbstract
{
	~AttackDataInfo() override = default;

	SGString name_;
	AttackDamageType_t attackDamageType_;
	float attackDamageRatio_;
	AttackXForceDirection_t attackXForceDir_;
	float attackXForce_;
	float attackYForce_;
	bool isFallDownAttack_;
};
