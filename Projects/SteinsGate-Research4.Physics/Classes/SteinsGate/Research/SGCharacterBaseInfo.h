/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:37:58 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGStruct.h>

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
	int JumpShotCount[WeaponType::Max];
	float AttackSpeed[WeaponType::Max];
	float JumpForce;
	float SlidingForce;
	int DefaultVisualZOrder[VisualType::Max];
	int DefaultVisualNpkIndex[VisualType::Max];
	int DefaultVisualImgIndex[VisualType::Max];
	SGThicknessBox ThicknessBox;
	WeaponType_t DefaultWeaponType;
};