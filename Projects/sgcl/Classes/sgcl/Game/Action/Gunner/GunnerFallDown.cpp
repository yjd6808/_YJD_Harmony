/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 10:33:18 AM
 * =====================
 *
 */

#include "Game/Action/Gunner/GunnerFallDown.h"

#include "sgcl/Game/Contents/HostPlayer.h"
#include "sgcl/Define/Define_Animation.h"
#include "sgcl/Game/Component/PhysicsComponent.h"

//////////////////////////////////////////////////////////////////////////////////////////
GunnerFallDown::GunnerFallDown(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
, bounced_(false)
, down_(false)
, elapsedDownTime_(0)
, downRecoverTime_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerFallDown::OnActionBegin()
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->EnableElasticity();

	pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_FALL_DOWN_BEGIN);
	elapsedDownTime_ = 0.0f;
	downRecoverTime_ = pBaseInfo_->downRecoverTime_ / 2.0f;
	bounced_ = false;
	down_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerFallDown::OnActionEnd()
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->DisableElasticity();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerFallDown::OnUpdate(float _deltaTime)
{
	Character* pCharacter = pPlayer_;
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	// Step 1. 바닥에 충돌해서 공중으로 튀어올랐는지 확인
	if (pPhysicsComponent && pPhysicsComponent->IsBounced() && !bounced_)
	{
		bounced_ = true;
		pCharacter->RunAnimation(DEF_ANIMATION_GUNNER_FALL_DOWN_BOUNCE);
		return;
	}

	// Step 2. 공중으로 튀어올랐다가 다시 바닥에 닿았는지 확인
	if (!down_ && bounced_ && pCharacter->IsOnTheGround())
	{
		pCharacter->RunAnimation(DEF_ANIMATION_GUNNER_FALL_DOWN_END);
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
		ActionMgr* pActionManager = pPlayer_->GetActionManager();

		pActionManager->StopActionForce();
		pActionManager->RunBaseAction(BaseAction::SitRecover);
	}
}
