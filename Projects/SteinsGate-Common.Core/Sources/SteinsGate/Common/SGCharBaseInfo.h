/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 12:21:52 AM
 * =====================
 *
 */


#pragma once

#include "Core.h"
#include <SteinsGate/Common/ConfigDataAbstract.h>

struct SGCharBaseInfo : ConfigDataAbstract
{
	~SGCharBaseInfo() override = default;

	// int Code;		// 캐릭터 타입
	int HP;
	int MP;
	int Strength;
	int Dexterity;
	int Vitality;
	int Intelligence;
	int HPLevelUp;
	int MPLevelUp;
	int LevelUpStat;
	float JumpForce;
	float SlidingForce;
	float DownRecoverTime;
	ThicknessBox ThicknessBox;
	WeaponType_t DefaultWeaponType;
};

struct SGCharInfo : SGCharBaseInfo
{
	~SGCharInfo() override = default;

	int DefaultVisualNpkIndex[VisualType::Max];
	int DefaultVisualImgIndex[VisualType::Max];
};

struct SGGunnerInfo : SGCharInfo
{
	~SGGunnerInfo() override = default;

	int ShotCount[GunnerWeaponType::Max];				// 지상 공격 횟수
	int JumpShotCount[GunnerWeaponType::Max];			// 점프 공격 횟수
	float AttackSpeed[GunnerWeaponType::Max];			// 공속
	float JumpShotForceX[GunnerWeaponType::Max];		// 점프 공격 X 반동
	float JumpShotForceY[GunnerWeaponType::Max];		// 점프 공격 Y 반동
	float JumpShotMoveSpeedX[GunnerWeaponType::Max];	// 점프 공격 X 반동스피드
};

