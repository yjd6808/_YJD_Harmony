/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:27 AM
 * =====================
 *
 */




#include "SGProjectile.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGActorSprite.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGProjectileListener.h>
#include <SteinsGate/Research/SGPlayer.h>

#include <SteinsGate/Common/Engine/RectPoly.h>

USING_NS_JC;
USING_NS_CC;

SGProjectile::SGProjectile(SGActor* spawner, SGProjectileInfo* baseInfo)
	: SGActor(ActorType::Projectile, baseInfo->Code)
	, m_fMoveDistance(0.0f)
	, m_fElapsedLifeTime(0.0f)
	, m_pSpwawner(spawner)
	, m_pBaseInfo(baseInfo)
	, m_bLifeTimeOver(false)
	, m_bDistanceOver(false)
	, m_bCollisionOnTheGround(false)
{}

SGProjectile::~SGProjectile() {
	CC_SAFE_RELEASE(m_pSpwawner);
}

SGProjectile* SGProjectile::create(SGActor* spawner, SGProjectileInfo* baseInfo) {
	SGProjectile* pProjectile = new SGProjectile(spawner, baseInfo);

	if (pProjectile && pProjectile->init()) {
		spawner->retain();
		pProjectile->initThicknessBox(baseInfo->ThicknessBox);
		pProjectile->initActorSprite();
		pProjectile->autorelease();
		return pProjectile;
	}

	return pProjectile;
}

void SGProjectile::initThicknessBox(const SGThicknessBox& thicknessBox) {
	SGActor::initThicknessBox(thicknessBox);

	// 두께빡스 위치 자동지정
	if (thicknessBox.RelativeY <= 0.0f) {
		SGVec2 spawnerCanvasPos = m_pSpwawner->getCanvasPositionReal();
		SGVec2 spawnerGroundPos = m_pSpwawner->getPositionRealCenter();

		// 스포너의 캔버스 기준상에서 프로젝틸의 절대 y 위치를 구한다.
		// 스포너의 절대 그라운드 위치 중앙 y 위치에서 빼주면 됨.
		float fRelativeY = spawnerGroundPos.y - (spawnerCanvasPos.y + m_pBaseInfo->SpawnOffsetY);
		m_pThicknessBox->setPositionY(fRelativeY);
	}
}

void SGProjectile::initListener(SGActorListener* listener) {
	DebugAssertMessage(m_pListener == nullptr, "이미 액터 리스너가 초기화 되어있습니다.");
	DebugAssertMessage(listener->getActorType() == ActorType::Projectile, "프로젝틸 리스너만 초기화 가능합니다.");
	m_pListener = listener;
}

// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void SGProjectile::initActorSprite() {
	SGDataManager* pDataManager = SGDataManager::getInstance();
	SGImagePackManager* pImgPackManager = SGImagePackManager::getInstance();
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>(1, m_pBaseInfo->AnimationList.Size());

	spActorSpriteData->Parts.PushBack({ 0, m_pBaseInfo->NpkIndex, m_pBaseInfo->ImgIndex });

	for (int i = 0; i < m_pBaseInfo->AnimationList.Size(); ++i) {
		spActorSpriteData->Animations.PushBack(m_pBaseInfo->AnimationList[i]);
	}

	m_pActorSprite = SGActorSprite::create(this, spActorSpriteData);
	m_pActorSprite->setAnchorPoint(Vec2::ZERO);
	m_pActorSprite->getBodyPart()->setRotation(
		m_pBaseInfo->Rotation + 
		SGRandom::random_real(m_pBaseInfo->RamdomRotationRangeMin, m_pBaseInfo->RamdomRotationRangeMax)
	);

	SGSize spawnerCanvsSize = m_pSpwawner->getCanvasSize();
	SGVec2 spawnerCanvasPos = m_pSpwawner->getCanvasPositionReal();

	setSpriteDirection(m_pSpwawner->getSpriteDirection());

	if (m_pSpwawner->getSpriteDirection() == SpriteDirection::Right) {
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
	m_pActorSprite->runAnimation(1);

	this->addChild(m_pActorSprite);
}



void SGProjectile::update(float dt) {
	SGActor::update(dt);

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

void SGProjectile::updateListenerEvent(float dt) {

	DebugAssert(m_pListener, "프로젝틸은 리스너 초기화가 필수입니다.");
	SGProjectileListener* pListener = (SGProjectileListener*)m_pListener;
	pListener->onUpdate(dt);

	if (isLifeTimeOver() && !m_bLifeTimeOver) {
		pListener->onLifeTimeOver();
		m_bLifeTimeOver = true;
	}

	if (isDistanceOver() && !m_bDistanceOver) {
		pListener->onDistanceOver();
		m_bDistanceOver = true;
	}

	if (isOnTheGround() && !m_bCollisionOnTheGround) {
		pListener->onCollisionWithGround();
		m_bCollisionOnTheGround = false;
	}
}

void SGProjectile::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pListener)
		m_pListener->onFrameBegin(animation, texture);
}

void SGProjectile::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pListener)
		m_pListener->onFrameEnd(animation, texture);
}

void SGProjectile::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pListener)
		m_pListener->onAnimationBegin(animation, texture);
}

void SGProjectile::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pListener)
		m_pListener->onAnimationEnd(animation, texture);
}


bool SGProjectile::isLifeTimeOver() {
	return m_bLifeTimeOver;
}

bool SGProjectile::isDistanceOver() {
	return m_bDistanceOver;
}

SGActor* SGProjectile::getSpawner() { return m_pSpwawner; }
SGProjectileInfo* SGProjectile::getBaseInfo() { return m_pBaseInfo; }
