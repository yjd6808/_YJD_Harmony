/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 8:32:39 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ProjectileInfo.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileInfo::ProjectileInfo(ProjectileInfo&& _other) noexcept
{
	this->operator=(Move(_other));
}

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileInfo& ProjectileInfo::operator=(ProjectileInfo&& _other) noexcept
{
	code_ = _other.code_;
	ListenerCode = _other.ListenerCode;
	AttackData = _other.AttackData;
	Name = Move(_other.Name);
	//SgaIndex = _other.SgaIndex;
	//ImgIndex = _other.ImgIndex;
	SpawnOffsetX = _other.SpawnOffsetX;
	SpawnOffsetY = _other.SpawnOffsetY;
	SpawnEffect = _other.SpawnEffect;
	SpawnEffectOffsetX = _other.SpawnEffectOffsetX;
	SpawnEffectOffsetY = _other.SpawnEffectOffsetY;
	HitEffect = _other.HitEffect;
	Rotation = _other.Rotation;
	RamdomRotationRangeMin = _other.RamdomRotationRangeMin;
	RamdomRotationRangeMax = _other.RamdomRotationRangeMax;
	Distance = _other.Distance;
	MoveSpeed = _other.MoveSpeed;
	LifeTime = _other.LifeTime;
	RehitDelay = _other.RehitDelay;
	IsSpriteDataRef = _other.IsSpriteDataRef;
	ThicknessBox = _other.ThicknessBox;
	SpriteData = _other.SpriteData;
	_other.SpriteData = nullptr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileInfo::~ProjectileInfo()
{
	if (IsSpriteDataRef)
		return;

	JCORE_DELETE_SAFE(SpriteData);
}
