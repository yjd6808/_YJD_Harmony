/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:50:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ProjectileListener.h"

#include <SteinsGate/Client/Projectile.h>

ProjectileListener::ProjectileListener()
	: m_fMoveDistance(0.0f)
	, m_fElapsedLifeTime(0.0f)
	, m_pProjectile(nullptr)
{}

void ProjectileListener::injectActor(Actor* actor) {
	DebugAssertMsg(actor->getType() == ActorType::Projectile, "프로젝틸 타입이 아닙니다.");
	m_pActor = actor;
	m_pProjectile = dynamic_cast<Projectile*>(actor);
}

void ProjectileListener::onCreated() {
	ActorListener::onCreated();

	m_fElapsedLifeTime = 0.0f;
	m_fMoveDistance = 0.0f;
}

void ProjectileListener::onUpdate(float dt) {
	DebugAssert(m_pActor);

	ActorSprite* pActorSprite = m_pActor->getActorSprite();

	const float currentRotation = pActorSprite->getBodyPart()->getRotation();
	const float fMoveSpeedFPS = m_pProjectile->getBaseInfo()->MoveSpeed / 60.0f;
	const float fMoveDistanceFPSX = fMoveSpeedFPS * cosf(CC_DEGREES_TO_RADIANS(currentRotation));
	const float fMoveDistanceFPSY = fMoveSpeedFPS * sinf(CC_DEGREES_TO_RADIANS(currentRotation));

	// x축 이동은 액터를 이동
	m_pActor->setPositionX(m_pActor->getPositionX() + (m_pActor->getSpriteDirection() == SpriteDirection::Right ? fMoveDistanceFPSX : -fMoveDistanceFPSX));

	// y축 이동은 엑터 스프라이트를 이동
	pActorSprite->setPositionY(pActorSprite->getPositionY() - fMoveDistanceFPSY);
	m_fMoveDistance += fMoveSpeedFPS;
	m_fElapsedLifeTime += dt;

	if (isLifeTimeOver()) {
		onLifeTimeOver();
	}

	if (isDistanceOver()) {
		onDistanceOver();
	}

	if (pActorSprite->getPositionY() <= m_pActor->getThicknessBoxNode()->getPositionY()) {
		onCollisionWithGround();
	}

}

void ProjectileListener::onCollisionWithGround() {
	m_pProjectile->cleanUpAtNextFrame();
}

void ProjectileListener::onLifeTimeOver() {
	m_pProjectile->cleanUpAtNextFrame();
}

void ProjectileListener::onDistanceOver() {
	m_pProjectile->cleanUpAtNextFrame();
}
