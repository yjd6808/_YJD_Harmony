/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:44:39 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct MobBaseInfo : ConfigDataAbstract
{
	virtual ~MobBaseInfo() = default;

	SGString Name;
	int HP;
	int MP;
	int PhysicalDamage;
	int MagicDamage;
	int Armor;
	int MagicArmor;
	float EnhancePerLevel;
	float Weight;
	float MoveSpeedX;
	float MoveSpeedY;
	float DownRecoverTime;
	ThicknessBox ThicknessBox;
};
