/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:50:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ProjectileListener.h"

#include <SteinsGate/Client/Projectile.h>

//////////////////////////////////////////////////////////////////////////////////////////
ProjectileListener::ProjectileListener(Projectile* _pProjectile, Actor* _pSpawner)
: m_pProjectile(_pProjectile)
, m_pSpawner(_pSpawner)
, m_fMoveDistance(0.0f)
, m_fElapsedLifeTime(0.0f)
{
}

void ProjectileListener::onCreated()
{
	IActorListener::onCreated();

	m_fElapsedLifeTime = 0.0f;
	m_fMoveDistance = 0.0f;
}

void ProjectileListener::onCleanUp()
{
	CC_SAFE_RELEASE_NULL(m_pSpawner);
}

void ProjectileListener::onUpdate(float _dt)
{
	ActorSprite* pActorSprite = m_pProjectile->getActorSprite();

	const float currentRotation = pActorSprite->getBodyPart()->getRotation();
	const float moveSpeedFps = m_pProjectile->getBaseInfo()->MoveSpeed / 60.0f;
	const float moveDistanceFpsX = moveSpeedFps * cosf(CC_DEGREES_TO_RADIANS(currentRotation));
	const float moveDistanceFpsY = moveSpeedFps * sinf(CC_DEGREES_TO_RADIANS(currentRotation));

	// x축 이동은 액터를 이동
	m_pProjectile->setPositionX(
		m_pProjectile->getPositionX() + (m_pProjectile->getSpriteDirection() == SpriteDirection::Right
			                                 ? moveDistanceFpsX
			                                 : -moveDistanceFpsX));

	// y축 이동은 엑터 스프라이트를 이동
	pActorSprite->setPositionY(pActorSprite->getPositionY() - moveDistanceFpsY);
	m_fMoveDistance += moveSpeedFps;
	m_fElapsedLifeTime += _dt;

	if (isLifeTimeOver())
	{
		onLifeTimeOver();
	}

	if (isDistanceOver())
	{
		onDistanceOver();
	}

	if (pActorSprite->getPositionY() <= m_pProjectile->getThicknessBoxNode()->getPositionY())
	{
		onCollisionWithGround();
	}
}

void ProjectileListener::onCollisionWithGround()
{
	m_pProjectile->cleanUpAtNextFrame();
}

void ProjectileListener::onLifeTimeOver()
{
	m_pProjectile->cleanUpAtNextFrame();
}

void ProjectileListener::onDistanceOver()
{
	m_pProjectile->cleanUpAtNextFrame();
}
