/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:27 AM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "Game/Actor/Projectile.h"

#include "sgcl/Game/Actor/ActorSprite.h"
#include "sgcl/Game/Listener/Projectile/ProjectileListener.h"


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

Projectile* Projectile::Create(ProjectileInfo* _pBaseInfo, Actor* _pSpawner)
{
	Projectile* pProjectile = dbg_new Projectile(_pBaseInfo);
	pProjectile->SetSpawner(_pSpawner);
	pProjectile->Initialize();
	pProjectile->autorelease();
	return pProjectile;
}

void Projectile::Initialize()
{
	InitThicknessBox(baseInfo_->thicknessBox_);
	InitActorSprite();
	InitHitRecorder(8, 16, spawner_);
	InitVariables();
	InitPosition();
	InitListeners();
	InitComponents();
}

void Projectile::InitThicknessBox(const ThicknessBox& _thicknessBox)
{
	Actor::InitThicknessBox(_thicknessBox);

	// 두께빡스 위치 자동지정
	if (_thicknessBox.relativeY_ <= 0.0f)
	{
		cc::vec2 spawnerCanvasPos = spawner_->GetCanvasPositionReal();
		cc::vec2 spawnerGroundPos = spawner_->GetPositionRealCenter();

		// 스포너의 캔버스 기준상에서 프로젝틸의 절대 y 위치를 구한다.
		// 스포너의 절대 그라운드 위치 중앙 y 위치에서 빼주면 됨.
		float relativeY = spawnerGroundPos.y - (spawnerCanvasPos.y + baseInfo_->spawnOffsetY_);
		pThicknessBox_->setPositionY(relativeY);
	}
}

void Projectile::InitPosition()
{
	cc::size spawnerCanvsSize = spawner_->GetCanvasSize();
	cc::vec2 spawnerCanvasPos = spawner_->GetCanvasPositionReal();

	SetSpriteDirection(spawner_->GetSpriteDirection());

	if (spawner_->GetSpriteDirection() == SpriteDirection::Right)
	{
		setPosition(
			spawnerCanvasPos.x + baseInfo_->spawnOffsetX_,
			spawnerCanvasPos.y + baseInfo_->spawnOffsetY_
		);
	}
	else
	{
		setPosition(
			spawnerCanvasPos.x + spawnerCanvsSize.width - baseInfo_->spawnOffsetX_,
			spawnerCanvasPos.y + baseInfo_->spawnOffsetY_
		);
	}

	pActorSprite_->setPosition(0, 0);
}

void Projectile::InitListeners()
{
	IActorListener* pListener = GetListener(IActorListener::Type::eProjectile);

	if (pListener == nullptr)
	{
		pListener = sg::Contents.ActorListenerManager->CreateProjectileListener(this, spawner_);
		AddListener(pListener);
	}
}

void Projectile::InitComponents()
{
}

// 프로젝틸은 파츠, 애니메이션 다 1개씩임
void Projectile::InitActorSprite()
{
	const float rotation = baseInfo_->rotation_ + g_cRandom.GenerateF(baseInfo_->ramdomRotationRangeMin_,
	                                                                   baseInfo_->ramdomRotationRangeMax_);

	if (!pActorSprite_)
	{
		pActorSprite_ = ActorSprite::Create(this, baseInfo_->spriteData_);
		pActorSprite_->setAnchorPoint(Vec2::ZERO);
		this->addChild(pActorSprite_);
	}

	pActorSprite_->GetBodyPart()->setRotation(rotation);
}

EffectInfo* Projectile::GetSpawnEffectInfo()
{
	jc_assert_msg(baseInfo_->spawnEffect_, "스폰 이펙트가 없습니다.");
	return baseInfo_->spawnEffect_;
}

EffectInfo* Projectile::GetHitEffectInfo()
{
	jc_assert_msg(baseInfo_->hitEffect_, "히트 이펙트가 없습니다.");
	return baseInfo_->hitEffect_;
}

void Projectile::SetSpawner(Actor* _pSpawner)
{
	CC_SAFE_RELEASE_NULL(spawner_);

	if (!_pSpawner)
		return;

	spawner_ = _pSpawner;
	spawner_->retain();

	if (pHitRecorder_)
		pHitRecorder_->SetOwner(_pSpawner);
}

Actor* Projectile::GetSpawner()
{
	return spawner_;
}

ProjectileInfo* Projectile::GetBaseInfo()
{
	return baseInfo_;
}
