/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 10:44:39 PM
 * =====================
 *
 */


#pragma once

#include "Core.h"
#include <SteinsGate/Common/ConfigDataAbstract.h>

struct SGMobBaseInfo : ConfigDataAbstract
{
	virtual ~SGMobBaseInfo() = default;

	SGString Name;
	int HP;
	int MP;
	int PhysicalDamage;
	int MagicalDamage;
	int Armor;
	int MagicArmor;
	float EnhancePerLevel;
	float Weight;
	float MoveSpeedX;
	float MoveSpeedY;
	float DownRecoverTime;
	SGThicknessBox ThicknessBox;
};
