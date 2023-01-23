/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:37:58 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/Config.h>

struct SGCharacterBaseInfo
{
	int Type;		// 캐릭터 타입
	int HP;
	int MP;
	int Strength;
	int Dexterity;
	int Vitality;
	int Intelligence;
	int HPLevelUp;
	int MPLevelUp;
	int LevelUpStat;
	int ShotCount[WeaponType::Max];
	float AttackSpeed[WeaponType::Max];
	float JumpForce;
	float SlidingForce;
	float ThicknessBoxWidth;
	float ThicknessBoxHeight;
	float ThicknessBoxRelativeY;
	int DefaultVisualZOrder[VisualType::Max];
	int DefaultVisualNpkIndex[VisualType::Max];
	int DefaultVisualImgIndex[VisualType::Max];
	WeaponType_t DefaultWeaponType;
};