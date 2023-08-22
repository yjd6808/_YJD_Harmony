/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 12:21:52 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>



struct CharBaseInfo : ConfigDataAbstract
{
	~CharBaseInfo() override = default;

	// 캐릭터 기본 정보
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

struct CharInfo : CharBaseInfo
{
	~CharInfo() override = default;

	VisualData Visual[VisualType::Max];
	int VisualCount[VisualType::Max];
	bool HasVisual[VisualType::Max];
};

struct GunnerInfo : CharInfo
{
	~GunnerInfo() override = default;

	int ShotCount[WeaponType::Max];				// 지상 공격 횟수
	int JumpShotCount[WeaponType::Max];			// 점프 공격 횟수
	float AttackSpeed[WeaponType::Max];			// 공속
	float JumpShotForceX[WeaponType::Max];		// 점프 공격 X 반동
	float JumpShotForceY[WeaponType::Max];		// 점프 공격 Y 반동
	float JumpShotMoveSpeedX[WeaponType::Max];	// 점프 공격 X 반동스피드
};

