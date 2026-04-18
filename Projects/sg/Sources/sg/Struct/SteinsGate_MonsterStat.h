/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:44:39 PM
 * =====================
 *
 */


#pragma once

#include "sg/Util/DescBase.h"

struct MonsterStatInfo : SDescBase
{
	~MonsterStatInfo() override = default;

	int hp_;
	int mp_;
	int physicalDamage_;
	int magicDamage_;
	int armor_;
	int magicArmor_;
	float weight_;
	float moveSpeedX_;
	float moveSpeedY_;
	float downRecoverTime_;
};
