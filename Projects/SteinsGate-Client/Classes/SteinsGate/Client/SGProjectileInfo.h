/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:10:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGAnimationInfo.h>

struct SGAttackDataInfo;
struct SGEffectInfo;
struct SGProjectileInfo
{
	SGProjectileInfo() = default;
	SGProjectileInfo(const SGProjectileInfo& other) = default;
	SGProjectileInfo(SGProjectileInfo&& other) noexcept;

	SGProjectileInfo& operator=(const SGProjectileInfo& other) = default;
	SGProjectileInfo& operator=(SGProjectileInfo&& other) noexcept;

	~SGProjectileInfo();

	int Code;
	int ProjectileListenerCode;
	SGAttackDataInfo* AttackData;
	SGString Name;
	int NpkIndex;
	int ImgIndex;
	float SpawnOffsetX;
	float SpawnOffsetY;
	SGEffectInfo* SpawnEffect;
	float SpawnEffectOffsetX;
	float SpawnEffectOffsetY;
	SGEffectInfo* HitEffect;
	float Rotation;
	float RamdomRotationRangeMin;
	float RamdomRotationRangeMax;
	float Distance;
	float MoveSpeed;
	float LifeTime;
	float RehitDelay;
	bool AnimationRef;	// 오버라이딩 추가에따라 애니메이션 원본인지 참조본인지 구분용도

	SGThicknessBox ThicknessBox;
	SGAnimationInfo* Animation;
};


