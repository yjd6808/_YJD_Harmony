/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGConfig.h>
#include "Tutturu.h"

struct SGAttackDataInfo
{
	int Code;
	SGString Name;
	AttackDamageType_t AttackDamageType;
	float AttackDamageRatio;
	AttackXForceDirection_t AttackXForceDir;
	float AttackXForce;
	float AttackYForce;
	bool IsFallDownAttack;
};