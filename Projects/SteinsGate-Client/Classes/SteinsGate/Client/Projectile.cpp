/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:27 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "Projectile.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/ProjectileListener.h>
#include <SteinsGate/Client/HostPlayer.h>

#include <SteinsGate/Common/RectPoly.h>

USING_NS_JC;
USING_NS_CC;

Projectile::Projectile(Actor* spawner, ProjectileInfo* baseInfo)
	: Actor(ActorType::Projectile, baseInfo->Code)
	, m_pSpawner(spawner)
	, m_pBaseInfo(baseInfo)
{}

Projectile::~Projectile() {
	CC_SAFE_RELEASE(m_pSpawner);
}

Projectile* Projectile::create(Actor* spawner, ProjectileInfo* baseInfo) {
	Projectile* pProjectile = dbg_new Projectile(spawner, baseInfo);

	spawner->retain();
	pProjectile->initThicknessBox(baseInfo->ThicknessBox);
	pProjectile->initActorSprite();
	pProjectile->initHitRecorder(8, 16, spawner);
	pProjectile->initVariables();
	pProjectile->initPosition();

	pProjectile->autorelease();
	return pProjectile;
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

// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void Projectile::initActorSprite() {
	DataManager* pDataManager = DataManager::Get();
	ImagePackManager* pImgPackManager = ImagePackManager::Get();
	//ActorSpriteDataPtr spActorSpriteData = MakeShared<ActorSpriteData>(1, 1);	// 프로젝틸도 파츠, 애니메이션 모두 한개

	//spActorSpriteData->Parts.PushBack({ 0, m_pBaseInfo->SgaIndex, m_pBaseInfo->ImgIndex });
	//spActorSpriteData->Animations.PushBack(m_pBaseInfo->Animation);

	m_pActorSprite = ActorSprite::create(this, m_pBaseInfo->SpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->getBodyPart()->setRotation(
		m_pBaseInfo->Rotation + 
		SGRandom::random_real(m_pBaseInfo->RamdomRotationRangeMin, m_pBaseInfo->RamdomRotationRangeMax)
	);

	this->addChild(m_pActorSprite);
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
	m_pSpawner = spawner;
}

Actor* Projectile::getSpawner() { return m_pSpawner; }
ProjectileInfo* Projectile::getBaseInfo() { return m_pBaseInfo; }
