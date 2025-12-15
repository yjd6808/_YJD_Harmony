/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 10:33:18 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerFallDown.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/PhysicsComponent.h>

//////////////////////////////////////////////////////////////////////////////////////////
GunnerFallDown::GunnerFallDown(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
, bounced_(false)
, down_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerFallDown::onActionBegin()
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->enableElasticity();

	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_FALL_DOWN_BEGIN);
	elapsedDownTime_ = 0.0f;
	downRecoverTime_ = m_pBaseInfo->downRecoverTime_ / 2.0f;
	bounced_ = false;
	down_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerFallDown::onActionEnd()
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->disableElasticity();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerFallDown::onUpdate(float _deltaTime)
{
	Character* pCharacter = m_pPlayer;
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	// Step 1. 바닥에 충돌해서 공중으로 튀어올랐는지 확인
	if (pPhysicsComponent && pPhysicsComponent->isBounced() && !bounced_)
	{
		bounced_ = true;
		pCharacter->runAnimation(DEF_ANIMATION_GUNNER_FALL_DOWN_BOUNCE);
		return;
	}

	// Step 2. 공중으로 튀어올랐다가 다시 바닥에 닿았는지 확인
	if (!down_ && bounced_ && pCharacter->isOnTheGround())
	{
		pCharacter->runAnimation(DEF_ANIMATION_GUNNER_FALL_DOWN_END);
		down_ = true;
		return;
	}

	// Step 3. 바닥에 누워있는 시간 경과 체크
	if (!down_)
	{
		return;
	}

	// if (죽어있는 경우)
	//   사망처리

	elapsedDownTime_ += _deltaTime;

	if (elapsedDownTime_ >= downRecoverTime_)
	{
		ActionMgr* pActionManager = m_pPlayer->actionManager();

		pActionManager->stopActionForce();
		pActionManager->runBaseAction(BaseAction::SitRecover);
	}
}
