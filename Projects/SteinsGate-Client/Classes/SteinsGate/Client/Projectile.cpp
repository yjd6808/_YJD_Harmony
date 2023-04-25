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
	, m_fMoveDistance(0.0f)
	, m_fElapsedLifeTime(0.0f)
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
	pProjectile->initHitRecorder(8, 16);
	pProjectile->initVariables();
	pProjectile->initPosition();

	pProjectile->autorelease();
	return pProjectile;
}

bool Projectile::initVariables() {
	Actor::initVariables();

	m_fMoveDistance = 0.0f;
	m_fElapsedLifeTime = 0.0f;

	return true;
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

void Projectile::initListener(ActorListener* listener) {
	DebugAssertMsg(m_pListener == nullptr, "이미 액터 리스너가 초기화 되어있습니다.");
	DebugAssertMsg(listener->getActorType() == ActorType::Projectile, "프로젝틸 리스너만 초기화 가능합니다.");
	m_pListener = listener;
	m_pListener->injectActor(this);
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
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(1, 1);	// 프로젝틸도 파츠, 애니메이션 모두 한개

	spActorSpriteData->Parts.PushBack({ 0, m_pBaseInfo->SgaIndex, m_pBaseInfo->ImgIndex });
	spActorSpriteData->Animations.PushBack(m_pBaseInfo->Animation);

	m_pActorSprite = ActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->getBodyPart()->setRotation(
		m_pBaseInfo->Rotation + 
		SGRandom::random_real(m_pBaseInfo->RamdomRotationRangeMin, m_pBaseInfo->RamdomRotationRangeMax)
	);

	this->addChild(m_pActorSprite);
}



void Projectile::update(float dt) {
	Actor::update(dt);

	float currentRotation = m_pActorSprite->getBodyPart()->getRotation();
	float fMoveSpeedFPS = m_pBaseInfo->MoveSpeed / 60.0f;
	float fMoveDistanceFPSX = fMoveSpeedFPS * cosf(CC_DEGREES_TO_RADIANS(currentRotation));
	float fMoveDistanceFPSY = fMoveSpeedFPS * sinf(CC_DEGREES_TO_RADIANS(currentRotation));

	// x축 이동은 액터를 이동
	this->setPositionX(getPositionX() + (getSpriteDirection() == SpriteDirection::Right ? fMoveDistanceFPSX : -fMoveDistanceFPSX));
	// y축 이동은 엑터 스프라이트를 이동
	m_pActorSprite->setPositionY(m_pActorSprite->getPositionY() - fMoveDistanceFPSY);
	m_fMoveDistance += fMoveSpeedFPS;
	m_fElapsedLifeTime += dt;

	updateListenerEvent(dt);
}

void Projectile::updateListenerEvent(float dt) {

	DebugAssertMsg(m_pListener, "프로젝틸은 리스너 초기화가 필수입니다.");
	ProjectileListener* pListener = (ProjectileListener*)m_pListener;
	pListener->onUpdate(dt);

	if (isLifeTimeOver()) {
		pListener->onLifeTimeOver();
	}

	if (isDistanceOver()) {
		pListener->onDistanceOver();
	}

	if (m_pActorSprite->getPositionY() <= m_pThicknessBox->getPositionY()) {
		pListener->onCollisionWithGround();
	}
}

void Projectile::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pListener)
		m_pListener->onFrameBegin(animation, texture);
}

void Projectile::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pListener)
		m_pListener->onFrameEnd(animation, texture);
}

void Projectile::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pListener)
		m_pListener->onAnimationBegin(animation, texture);
}

void Projectile::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pListener)
		m_pListener->onAnimationEnd(animation, texture);
}

EffectInfo* Projectile::getSpawnEffectInfo() {
	DebugAssertMsg(m_pBaseInfo->SpawnEffect, "스폰 이펙트가 없습니다.");
	return m_pBaseInfo->SpawnEffect; 
}

EffectInfo* Projectile::getHitEffectInfo() {
	DebugAssertMsg(m_pBaseInfo->HitEffect, "히트 이펙트가 없습니다.");
	return m_pBaseInfo->HitEffect; 
}


bool Projectile::isLifeTimeOver() {
	return m_fElapsedLifeTime >= m_pBaseInfo->LifeTime;
}

bool Projectile::isDistanceOver() {
	return m_fMoveDistance >= m_pBaseInfo->Distance;
}

void Projectile::setSpawner(Actor* spawner) {
	m_pSpawner = spawner;
}

Actor* Projectile::getSpawner() { return m_pSpawner; }
ProjectileInfo* Projectile::getBaseInfo() { return m_pBaseInfo; }
