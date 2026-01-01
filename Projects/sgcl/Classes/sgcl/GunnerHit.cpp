/*
 * 작성자: 윤정도
 * 생성일: 1/27/2023 8:03:49 AM
 * =====================
 *
 */

#include "Core.h"
#include "GunnerHit.h"

#include <sgcl/HostPlayer.h>
#include <sgcl/Define_Animation.h>
#include <sgcl/PhysicsComponent.h>

//////////////////////////////////////////////////////////////////////////////////////////
GunnerHit::GunnerHit(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
, hitSmall_(false)
, onTheGround_(false)
, downTimeCheckBegin_(false)
, elapsedDownTime_(0)
, downRecoverTime_(0)
// 초기 Hit 판정시 공중이었는지 아니면 바닥이었는지
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::OnActionBegin()
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->EnableElasticity();

	elapsedDownTime_ = 0.0f;
	downTimeCheckBegin_ = false;
	downRecoverTime_ = pBaseInfo_->downRecoverTime_ / 2.0f;

	SelectHitAnimation();
	CheckPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::OnActionEnd()
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent)
		pPhysicsComponent->DisableElasticity();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::OnUpdate(float _deltaTime)
{
	if (onTheGround_)
	{
		UpdateGroundHitState(_deltaTime);
		return;
	}

	UpdateAirHitState(_deltaTime);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::SelectHitAnimation()
{
	if (hitSmall_)
	{
		pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_HIT_SMALL);
	}
	else
	{
		pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_HIT_BIG);
	}

	hitSmall_ = !hitSmall_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::CheckPosition()
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (!pPhysicsComponent->HasForceY() && pPlayer_->GetPositionActorY() <= SG_FLT_EPSILON)
	{
		onTheGround_ = true;
		return;
	}

	onTheGround_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::UpdateGroundHitState(float _deltaTime)
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent && pPhysicsComponent->HasForceX())
		return;

	// TODO: 죽음 확인 후 사망처리
	Stop();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::UpdateAirHitState(float _deltaTime)
{
	if (!pPlayer_->IsOnTheGround())
		return;

	// TODO: 죽음 확인 후 사망처리

	if (!downTimeCheckBegin_)
	{
		pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_FALL_DOWN_END);
		downTimeCheckBegin_ = true;
		return;
	}

	UpdateDownState(_deltaTime);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerHit::UpdateDownState(float _deltaTime)
{
	elapsedDownTime_ += _deltaTime;

	if (elapsedDownTime_ >= downRecoverTime_)
	{
		ActionMgr* pActionManager = pPlayer_->GetActionManager();

		pActionManager->StopActionForce();
		pActionManager->RunBaseAction(BaseAction::SitRecover);
	}
}
