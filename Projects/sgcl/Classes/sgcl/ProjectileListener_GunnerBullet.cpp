/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:53:48 PM
 * =====================
 *
 */

#include "Core.h"
#include "ProjectileListener_GunnerBullet.h"

#include <sgcl/Projectile.h>
#include <sgcl/HostPlayer.h>
#include <sgcl/ActorPartAnimation.h>
#include <sg/_Struct/SteinsGate_AttackData.h>
#include <sgcl/Define_Effect.h>
#include <sgcl/Define_Event.h>
#include <sgcl/ActorManager.h>

ProjectileListener_GunnerBullet::ProjectileListener_GunnerBullet(Projectile* _pProjectile, Actor* _pSpawner)
: ProjectileListener(_pProjectile, _pSpawner)
{
}

void ProjectileListener_GunnerBullet::OnCreated()
{
	ProjectileListener::OnCreated();
	HitRecorder* pHitRecorder = pProjectile_->GetHitRecorder();

	pHitRecorder->ClearAlreadyHitEnemies();
	pHitRecorder->SetAlreadyHitRecord(true);
	pHitRecorder->AddSingleHitCallback(
		DEF_EVENT_HIT_GUNNER_BULLET, CC_CALLBACK_1(ProjectileListener_GunnerBullet::onEnemySingleHit, this));
	pHitRecorder->AddMultiHitCallback(
		DEF_EVENT_HIT_GUNNER_BULLET, CC_CALLBACK_2(ProjectileListener_GunnerBullet::onEnemyMultiHit, this));
}

void ProjectileListener_GunnerBullet::OnUpdate(float _dt)
{
	ProjectileListener::OnUpdate(_dt);

	const ActorRect& projectileActorRect = pProjectile_->GetActorRect();
	const int attackDataCode = pProjectile_->GetBaseInfo()->attackData_->code_;

	pProjectile_->GetHitRecorder()->Record(projectileActorRect, attackDataCode);
}

void ProjectileListener_GunnerBullet::OnCollisionWithGround()
{
	ProjectileListener::OnCollisionWithGround();
	ActorManager::Get()->CreateEffectOnMapAbsolute(
		DEF_EFFECT_COLLISION_FLOOR,
		pProjectile_->GetPositionRealCenterX(),
		pProjectile_->GetPositionRealCenterY(),
		pProjectile_->getLocalZOrder() + 1
	);
}

void ProjectileListener_GunnerBullet::onEnemySingleHit(HitInfo& _info)
{
	if (pProjectile_->GetHitRecorder()->IsAlreadyHit(_info.pHitTarget_))
		return;

	EffectInfo* pHitEffectInfo = pProjectile_->GetHitEffectInfo();
	ActorManager::Get()->CreateEffectOnMapTargetCollision(
		pHitEffectInfo->code_,
		SpriteDirection::Reverse[_info.hitDirection_],
		_info);
	_info.pHitTarget_->Hit(_info);
	pProjectile_->CleanUpAtNextFrame();
}

void ProjectileListener_GunnerBullet::onEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount)
{
}
