/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:10:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGActionInfo.h>

struct SGProjectileInfo
{
	SGProjectileInfo(int animationSize)
		: AnimationList(animationSize) {}

	SGProjectileInfo(const SGProjectileInfo& other) = default;
	SGProjectileInfo(SGProjectileInfo&& other) = default;

	SGProjectileInfo& operator=(const SGProjectileInfo& other) = default;
	SGProjectileInfo& operator=(SGProjectileInfo&& other) = default;

	~SGProjectileInfo() {

		if (AnimationRef) 
			return;

		for (int i = 0; i < AnimationList.Size(); ++i) {
			DeleteSafe(AnimationList[i]);
		}
	}

	int Code;
	SGString Name;
	int NpkIndex;
	int ImgIndex;
	float SpawnOffsetX;
	float SpawnOffsetY;
	int SpawnEffectCode;
	int HitEffectCode;
	float Rotation;
	float RamdomRotationRangeMin;
	float RamdomRotationRangeMax;
	int AttackDamageType;
	float AttackDamageRatio;
	float AttackXForce;
	float AttackYForce;
	float Distance;
	float MoveSpeed;
	float LifeTime;
	float RehitDelay;
	bool AnimationRef;	// 오버라이딩 추가에따라 애니메이션 원본인지 참조본인지 가에따라 애니메이션 원본인지 참조본인지 구분용도

	SGThicknessBox ThicknessBox;
	SGVector<SGAnimationInfo*> AnimationList;
};