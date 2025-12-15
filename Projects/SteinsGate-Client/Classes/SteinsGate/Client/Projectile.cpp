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

//////////////////////////////////////////////////////////////////////////////////////////
Projectile::Projectile(ProjectileInfo* _pBaseInfo)
: baseInfo_(_pBaseInfo)
, spawner_(nullptr)
{
}

Projectile::~Projectile()
{
	CC_SAFE_RELEASE_NULL(spawner_);
}

Projectile* Projectile::create(ProjectileInfo* _pBaseInfo, Actor* _pSpawner)
{
	Projectile* pProjectile = dbg_new Projectile(_pBaseInfo);
	pProjectile->setSpawner(_pSpawner);
	pProjectile->initialize();
	pProjectile->autorelease();
	return pProjectile;
}

void Projectile::initialize()
{
	initThicknessBox(baseInfo_->ThicknessBox);
	initActorSprite();
	initHitRecorder(8, 16, spawner_);
	initVariables();
	initPosition();
	initListeners();
	initComponents();
}

void Projectile::initThicknessBox(const ThicknessBox& _thicknessBox)
{
	Actor::initThicknessBox(_thicknessBox);

	// 두께빡스 위치 자동지정
	if (_thicknessBox.RelativeY <= 0.0f)
	{
		SGVec2 spawnerCanvasPos = spawner_->getCanvasPositionReal();
		SGVec2 spawnerGroundPos = spawner_->getPositionRealCenter();

		// 스포너의 캔버스 기준상에서 프로젝틸의 절대 y 위치를 구한다.
		// 스포너의 절대 그라운드 위치 중앙 y 위치에서 빼주면 됨.
		float relativeY = spawnerGroundPos.y - (spawnerCanvasPos.y + baseInfo_->SpawnOffsetY);
		m_pThicknessBox->setPositionY(relativeY);
	}
}

void Projectile::initPosition()
{
	SGSize spawnerCanvsSize = spawner_->getCanvasSize();
	SGVec2 spawnerCanvasPos = spawner_->getCanvasPositionReal();

	setSpriteDirection(spawner_->getSpriteDirection());

	if (spawner_->getSpriteDirection() == SpriteDirection::Right)
	{
		setPosition(
			spawnerCanvasPos.x + baseInfo_->SpawnOffsetX,
			spawnerCanvasPos.y + baseInfo_->SpawnOffsetY
		);
	}
	else
	{
		setPosition(
			spawnerCanvasPos.x + spawnerCanvsSize.width - baseInfo_->SpawnOffsetX,
			spawnerCanvasPos.y + baseInfo_->SpawnOffsetY
		);
	}

	m_pActorSprite->setPosition(0, 0);
}

void Projectile::initListeners()
{
	IActorListener* pListener = getListener(IActorListener::Type::eProjectile);

	if (pListener == nullptr)
	{
		pListener = Core::Contents.ActorListenerManager->createProjectileListener(this, spawner_);
		addListener(pListener);
	}
}

void Projectile::initComponents()
{
}

// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void Projectile::initActorSprite()
{
	const float rotation = baseInfo_->Rotation + SGRandom::random_real(baseInfo_->RamdomRotationRangeMin,
	                                                                   baseInfo_->RamdomRotationRangeMax);

	if (!m_pActorSprite)
	{
		m_pActorSprite = ActorSprite::create(this, baseInfo_->SpriteData);
		m_pActorSprite->setAnchorPoint(Vec2::ZERO);
		this->addChild(m_pActorSprite);
	}

	m_pActorSprite->getBodyPart()->setRotation(rotation);
}

EffectInfo* Projectile::getSpawnEffectInfo()
{
	DebugAssertMsg(baseInfo_->SpawnEffect, "스폰 이펙트가 없습니다.");
	return baseInfo_->SpawnEffect;
}

EffectInfo* Projectile::getHitEffectInfo()
{
	DebugAssertMsg(baseInfo_->HitEffect, "히트 이펙트가 없습니다.");
	return baseInfo_->HitEffect;
}

void Projectile::setSpawner(Actor* _pSpawner)
{
	CC_SAFE_RELEASE_NULL(spawner_);

	if (!_pSpawner)
		return;

	spawner_ = _pSpawner;
	spawner_->retain();

	if (m_pHitRecorder)
		m_pHitRecorder->setOwner(_pSpawner);
}

Actor* Projectile::getSpawner()
{
	return spawner_;
}

ProjectileInfo* Projectile::getBaseInfo()
{
	return baseInfo_;
}
