/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:50:11 PM
 * =====================
 *
 */

#include "Core.h"
#include "ProjectileListener.h"

#include <sgcl/Projectile.h>

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileListener::ProjectileListener(Projectile* _pProjectile, Actor* _pSpawner)
: pProjectile_(_pProjectile)
, pSpawner_(_pSpawner)
, moveDistance_(0.0f)
, elapsedLifeTime_(0.0f)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProjectileListener::OnCreated()
{
	IActorListener::OnCreated();

	elapsedLifeTime_ = 0.0f;
	moveDistance_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProjectileListener::OnCleanUp()
{
	CC_SAFE_RELEASE_NULL(pSpawner_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProjectileListener::OnUpdate(float _dt)
{
	ActorSprite* pActorSprite = pProjectile_->GetActorSprite();

	const float currentRotation = pActorSprite->GetBodyPart()->getRotation();
	const float moveSpeedFps = pProjectile_->GetBaseInfo()->moveSpeed_ / 60.0f;
	const float moveDistanceFpsX = moveSpeedFps * cosf(CC_DEGREES_TO_RADIANS(currentRotation));
	const float moveDistanceFpsY = moveSpeedFps * sinf(CC_DEGREES_TO_RADIANS(currentRotation));

	// x축 이동은 액터를 이동
	pProjectile_->setPositionX(
		pProjectile_->getPositionX() + (pProjectile_->GetSpriteDirection() == SpriteDirection::Right
			                                 ? moveDistanceFpsX
			                                 : -moveDistanceFpsX));

	// y축 이동은 엑터 스프라이트를 이동
	pActorSprite->setPositionY(pActorSprite->getPositionY() - moveDistanceFpsY);
	moveDistance_ += moveSpeedFps;
	elapsedLifeTime_ += _dt;

	if (IsLifeTimeOver())
	{
		OnLifeTimeOver();
	}

	if (IsDistanceOver())
	{
		OnDistanceOver();
	}

	if (pActorSprite->getPositionY() <= pProjectile_->GetThicknessBoxNode()->getPositionY())
	{
		OnCollisionWithGround();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProjectileListener::OnCollisionWithGround()
{
	pProjectile_->CleanUpAtNextFrame();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProjectileListener::OnLifeTimeOver()
{
	pProjectile_->CleanUpAtNextFrame();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProjectileListener::OnDistanceOver()
{
	pProjectile_->CleanUpAtNextFrame();
}
