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
	// int Code;        // 캐릭터 타입
	int hp_;
	int mp_;
	int strength_;
	int dexterity_;
	int vitality_;
	int intelligence_;
	int hpLevelUp_;
	int mpLevelUp_;
	int levelUpStat_;
	float jumpForce_;
	float slidingForce_;
	float downRecoverTime_;
	ThicknessBox thicknessBox_;
	WeaponType_t defaultWeaponType_;
};

struct CharInfo : CharBaseInfo
{
	~CharInfo() override = default;

	VisualData visual_[VisualType::Max];
	int visualCount_[VisualType::Max];
	bool hasVisual_[VisualType::Max];
};

struct GunnerInfo : CharInfo
{
	~GunnerInfo() override = default;

	int shotCount_[WeaponType::Max]; // 지상 공격 횟수
	int jumpShotCount_[WeaponType::Max]; // 점프 공격 횟수
	float attackSpeed_[WeaponType::Max]; // 공속
	float jumpShotForceX_[WeaponType::Max]; // 점프 공격 X 반동
	float jumpShotForceY_[WeaponType::Max]; // 점프 공격 Y 반동
	float jumpShotMoveSpeedX_[WeaponType::Max]; // 점프 공격 X 반동스피드
};
