/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:46:28 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGAnimationInfo.h>
#include <SteinsGate/Research/Config.h>

struct SGMonsterPartInfo
{
	int PackIndex;
	int ImgIndex;
	int ZOrder;
};

struct SGMonsterInfo
{
	SGMonsterInfo(int animationSize) : Animations(animationSize) {
	}

	int Code;
	SGString Name;
	int PackIndex;
	int SkinImgIndex;
	int PartsIndex[MaxMonsterPartsCount_v];
	int HP;
	int MP;
	int PhysicalDamage;
	int MagicalDamage;
	int Armor;
	int MagicArmor;
	float EnhancePerLevel;
	float Weight;
	int PartsCount;
	float ThicknessBoxWidth;
	float ThicknessBoxHeight;
	float ThicknessBoxRelativeY;
	SGMonsterPartInfo Parts[MaxMonsterPartsCount_v];
	SGVector<SGAnimationInfo> Animations;
};
