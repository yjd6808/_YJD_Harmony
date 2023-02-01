/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:37:58 AM
 * =====================
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
	float JumpForce;
	float SlidingForce;
	float DownRecoverTime;
	int DefaultVisualZOrder[VisualType::Max];
	int DefaultVisualNpkIndex[VisualType::Max];
	int DefaultVisualImgIndex[VisualType::Max];
	SGThicknessBox ThicknessBox;
	WeaponType_t DefaultWeaponType;
};


struct SGGunnerBaseInfo : SGCharacterBaseInfo
{
	int ShotCount[GunnerWeaponType::Max];				// 지상 공격 횟수
	int JumpShotCount[GunnerWeaponType::Max];			// 점프 공격 횟수
	float AttackSpeed[GunnerWeaponType::Max];			// 공속
	float JumpShotForceX[GunnerWeaponType::Max];		// 점프 공격 X 반동
	float JumpShotForceY[GunnerWeaponType::Max];		// 점프 공격 Y 반동
	float JumpShotMoveSpeedX[GunnerWeaponType::Max];	// 점프 공격 X 반동스피드
};