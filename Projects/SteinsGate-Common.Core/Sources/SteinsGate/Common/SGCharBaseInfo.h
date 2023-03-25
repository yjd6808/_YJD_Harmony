/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 12:21:52 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>



struct SGCharBaseInfo : ConfigDataAbstract
{
	~SGCharBaseInfo() override = default;

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

struct SGCharInfo : SGCharBaseInfo
{
	~SGCharInfo() override = default;

	VisualData Visual[VisualType::Max];
	int VisualCount[VisualType::Max];
	bool HasVisual[VisualType::Max];
};

struct SGGunnerInfo : SGCharInfo
{
	~SGGunnerInfo() override = default;

	int ShotCount[CharWeaponTypeCount_v];				// 지상 공격 횟수
	int JumpShotCount[CharWeaponTypeCount_v];			// 점프 공격 횟수
	float AttackSpeed[CharWeaponTypeCount_v];			// 공속
	float JumpShotForceX[CharWeaponTypeCount_v];		// 점프 공격 X 반동
	float JumpShotForceY[CharWeaponTypeCount_v];		// 점프 공격 Y 반동
	float JumpShotMoveSpeedX[CharWeaponTypeCount_v];	// 점프 공격 X 반동스피드
};

