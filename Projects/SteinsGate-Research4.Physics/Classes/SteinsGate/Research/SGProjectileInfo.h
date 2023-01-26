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

	int Code;
	SGString Name;
	int NpkIndex;
	int ImgIndex;
	float SpawnOffsetX;
	float SpawnOffsetY;
	int SpawnEffectCode;
	int HitEffectCode;
	int Rotation;
	int AttackDamageType;
	float AttackDamageRatio;
	float AttackXForce;
	float AttackYForce;
	bool ApplyPhysics;
	float Weight;
	float Distance;
	float MoveSpeed;
	float LifeTime;
	float PenetrationRate;
	float RehitDelay;
	SGThicknessBox ThicknessBox;
	SGVector<SGAnimationInfo> AnimationList;
};