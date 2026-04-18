/*
 * 작성자: 윤정도
 * 생성일: 2/3/2023 8:32:39 PM
 * =====================
 *
 */

#include "SteinsGate_Projectile.h"

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
	listenerCode_ = _other.listenerCode_;
	attackData_ = _other.attackData_;
	name_ = Move(_other.name_);
	//SgaIndex = _other.SgaIndex;
	//ImgIndex = _other.ImgIndex;
	spawnOffsetX_ = _other.spawnOffsetX_;
	spawnOffsetY_ = _other.spawnOffsetY_;
	spawnEffect_ = _other.spawnEffect_;
	spawnEffectOffsetX_ = _other.spawnEffectOffsetX_;
	spawnEffectOffsetY_ = _other.spawnEffectOffsetY_;
	hitEffect_ = _other.hitEffect_;
	rotation_ = _other.rotation_;
	ramdomRotationRangeMin_ = _other.ramdomRotationRangeMin_;
	ramdomRotationRangeMax_ = _other.ramdomRotationRangeMax_;
	distance_ = _other.distance_;
	moveSpeed_ = _other.moveSpeed_;
	lifeTime_ = _other.lifeTime_;
	rehitDelay_ = _other.rehitDelay_;
	isSpriteDataRef_ = _other.isSpriteDataRef_;
	thicknessBox_ = _other.thicknessBox_;
	spriteData_ = _other.spriteData_;
	_other.spriteData_ = nullptr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileInfo::~ProjectileInfo()
{
	if (isSpriteDataRef_)
		return;

	JC_DELETE_SAFE(spriteData_);
}
