/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:44:39 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct MonsterStatInfo : ConfigDataAbstract
{
	~MonsterStatInfo() override = default;

	int HP;
	int MP;
	int PhysicalDamage;
	int MagicDamage;
	int Armor;
	int MagicArmor;
	float Weight;
	float MoveSpeedX;
	float MoveSpeedY;
	float DownRecoverTime;
};
