﻿/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 8:32:39 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGProjectileInfo.h"

USING_NS_JC;

SGProjectileInfo::SGProjectileInfo(SGProjectileInfo&& other) noexcept {
	this->operator=(Move(other));
}

SGProjectileInfo& SGProjectileInfo::operator=(SGProjectileInfo&& other) noexcept {
	Code = other.Code;
	ProjectileListenerCode = other.ProjectileListenerCode;
	AttackData = other.AttackData;
	Name = Move(other.Name);
	NpkIndex = other.NpkIndex;
	ImgIndex = other.ImgIndex;
	SpawnOffsetX = other.SpawnOffsetX;
	SpawnOffsetY = other.SpawnOffsetY;
	SpawnEffect = other.SpawnEffect;
	SpawnEffectOffsetX = other.SpawnEffectOffsetX;
	SpawnEffectOffsetY = other.SpawnEffectOffsetY;
	HitEffect = other.HitEffect;
	Rotation = other.Rotation;
	RamdomRotationRangeMin = other.RamdomRotationRangeMin;
	RamdomRotationRangeMax = other.RamdomRotationRangeMax;
	Distance = other.Distance;
	MoveSpeed = other.MoveSpeed;
	LifeTime = other.LifeTime;
	RehitDelay = other.RehitDelay;
	AnimationRef = other.AnimationRef;
	ThicknessBox = other.ThicknessBox;
	Animation = other.Animation;
	other.Animation = nullptr;
	return *this;
}

SGProjectileInfo::~SGProjectileInfo() {
	if (AnimationRef)
		return;

	DeleteSafe(Animation);
}
