/*
 * 작성자: 윤정도
 * 생성일: 1/27/2023 8:03:49 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerHit.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/PhysicsComponent.h>

//////////////////////////////////////////////////////////////////////////////////////////
GunnerHit::GunnerHit(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
, hitSmall_(false)
, onTheGround_(false) // 초기 Hit 판정시 공중이었는지 아니면 바닥이었는지
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::onActionBegin()
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->enableElasticity();

	elapsedDownTime_ = 0.0f;
	downTimeCheckBegin_ = false;
	downRecoverTime_ = m_pBaseInfo->downRecoverTime_ / 2.0f;

	selectHitAnimation();
	checkPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::onActionEnd()
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->disableElasticity();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::onUpdate(float _deltaTime)
{
	if (onTheGround_)
	{
		updateGroundHitState(_deltaTime);
		return;
	}

	updateAirHitState(_deltaTime);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::selectHitAnimation()
{
	if (hitSmall_)
	{
		m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_HIT_SMALL);
	}
	else
	{
		m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_HIT_BIG);
	}

	hitSmall_ = !hitSmall_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::checkPosition()
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (!pPhysicsComponent->hasForceY() && m_pPlayer->getPositionActorY() <= SG_FLT_EPSILON)
	{
		onTheGround_ = true;
		return;
	}

	onTheGround_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::updateGroundHitState(float _deltaTime)
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent && pPhysicsComponent->hasForceX())
		return;

	// TODO: 죽음 확인 후 사망처리
	stop();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::updateAirHitState(float _deltaTime)
{
	if (!m_pPlayer->isOnTheGround())
		return;

	// TODO: 죽음 확인 후 사망처리

	if (!downTimeCheckBegin_)
	{
		m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_FALL_DOWN_END);
		downTimeCheckBegin_ = true;
		return;
	}

	updateDownState(_deltaTime);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::updateDownState(float _deltaTime)
{
	elapsedDownTime_ += _deltaTime;

	if (elapsedDownTime_ >= downRecoverTime_)
	{
		ActionMgr* pActionManager = m_pPlayer->actionManager();

		pActionManager->stopActionForce();
		pActionManager->runBaseAction(BaseAction::SitRecover);
	}
}
