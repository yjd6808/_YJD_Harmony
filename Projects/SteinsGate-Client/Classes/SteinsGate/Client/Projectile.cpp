/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:27 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "Projectile.h"

#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/ProjectileListener.h>

#include <SteinsGate/Common/RectPoly.h>

USING_NS_JC;
USING_NS_CC;

Projectile::Projectile(ProjectileInfo* baseInfo)
	: m_pBaseInfo(baseInfo)
	, m_pSpawner(nullptr)
{}

Projectile::~Projectile() {
	CC_SAFE_RELEASE_NULL(m_pSpawner);
}

Projectile* Projectile::create(ProjectileInfo* baseInfo, Actor* spawner) {
	Projectile* pProjectile = dbg_new Projectile(baseInfo);
	pProjectile->setSpawner(spawner);
	pProjectile->initialize();
	pProjectile->autorelease();
	return pProjectile;
}

void Projectile::initialize() {
	initThicknessBox(m_pBaseInfo->ThicknessBox);
	initActorSprite();
	initHitRecorder(8, 16, m_pSpawner);
	initVariables();
	initPosition();
	initListeners();
	initComponents();
}

void Projectile::initThicknessBox(const ThicknessBox& thicknessBox) {
	Actor::initThicknessBox(thicknessBox);

	// 두께빡스 위치 자동지정
	if (thicknessBox.RelativeY <= 0.0f) {
		SGVec2 spawnerCanvasPos = m_pSpawner->getCanvasPositionReal();
		SGVec2 spawnerGroundPos = m_pSpawner->getPositionRealCenter();

		// 스포너의 캔버스 기준상에서 프로젝틸의 절대 y 위치를 구한다.
		// 스포너의 절대 그라운드 위치 중앙 y 위치에서 빼주면 됨.
		float fRelativeY = spawnerGroundPos.y - (spawnerCanvasPos.y + m_pBaseInfo->SpawnOffsetY);
		m_pThicknessBox->setPositionY(fRelativeY);
	}
}

void Projectile::initPosition() {
	SGSize spawnerCanvsSize = m_pSpawner->getCanvasSize();
	SGVec2 spawnerCanvasPos = m_pSpawner->getCanvasPositionReal();

	setSpriteDirection(m_pSpawner->getSpriteDirection());

	if (m_pSpawner->getSpriteDirection() == SpriteDirection::Right) {
		setPosition(
			spawnerCanvasPos.x + m_pBaseInfo->SpawnOffsetX,
			spawnerCanvasPos.y + m_pBaseInfo->SpawnOffsetY
		);
	} else {
		setPosition(
			spawnerCanvasPos.x + spawnerCanvsSize.width - m_pBaseInfo->SpawnOffsetX,
			spawnerCanvasPos.y + m_pBaseInfo->SpawnOffsetY
		);
	}



	m_pActorSprite->setPosition(0, 0);
}

void Projectile::initListeners() {

	IActorListener* pListener = getListener(IActorListener::Type::eProjectile);

	if (pListener == nullptr) {
		pListener = Core::Contents.ActorListenerManager->createProjectileListener(this, m_pSpawner);
		addListener(pListener);
	}
}

void Projectile::initComponents() {

}

// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void Projectile::initActorSprite() {

	const float fRotation = m_pBaseInfo->Rotation + SGRandom::random_real(m_pBaseInfo->RamdomRotationRangeMin, m_pBaseInfo->RamdomRotationRangeMax);

	if (!m_pActorSprite) {
		m_pActorSprite = ActorSprite::create(this, m_pBaseInfo->SpriteData);
		m_pActorSprite->setAnchorPoint(Vec2::ZERO);
		this->addChild(m_pActorSprite);
	}

	m_pActorSprite->getBodyPart()->setRotation(fRotation);
}

EffectInfo* Projectile::getSpawnEffectInfo() {
	DebugAssertMsg(m_pBaseInfo->SpawnEffect, "스폰 이펙트가 없습니다.");
	return m_pBaseInfo->SpawnEffect; 
}

EffectInfo* Projectile::getHitEffectInfo() {
	DebugAssertMsg(m_pBaseInfo->HitEffect, "히트 이펙트가 없습니다.");
	return m_pBaseInfo->HitEffect; 
}

void Projectile::setSpawner(Actor* spawner) {
	CC_SAFE_RELEASE_NULL(m_pSpawner);

	if (!spawner)
		return;

	m_pSpawner = spawner;
	m_pSpawner->retain();

	if (m_pHitRecorder)
		m_pHitRecorder->setOwner(spawner);
}

Actor* Projectile::getSpawner() { return m_pSpawner; }
ProjectileInfo* Projectile::getBaseInfo() { return m_pBaseInfo; }
