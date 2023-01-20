/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:46:28 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGAnimationInfo.h>
#include <SteinsGate/Research/Config.h>

struct SGMonsterInfo
{
	int Code;
	int PackIndex;
	int SkinImgIndex;
	int PartsIndex[MaxMonsterPartsCount_v];
	int HP;
	int MP;
	int PhysicalAttackDamage;
	int Armor;
	int MagicArmor;
	float EnhancePerLevel;
	float Weight;
	SGVector<SGAnimationInfo> Animations;
};
