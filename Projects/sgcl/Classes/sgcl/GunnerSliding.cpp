/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Core.h"
#include "GunnerSliding.h"

#include <sgcl/HostPlayer.h>
#include <sgcl/Define_Action.h>
#include <sgcl/Define_Animation.h>
#include <sgcl/Define_Effect.h>
#include <sgcl/Define_Event.h>
#include <sgcl/ActorManager.h>
#include <sgcl/PhysicsComponent.h>

//////////////////////////////////////////////////////////////////////////////////////////
GunnerSliding::GunnerSliding(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

bool GunnerSliding::OnConditionCheck()
{
	return pPlayer_->GetRunningActionCode() == DEF_ACTION_GUNNER_RUN;
}

void GunnerSliding::OnActionBegin()
{
	slidingStarted_ = false;
	pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_SLIDING);
	pHitRecorder_->SetAlreadyHitRecord(true);
	pHitRecorder_->AddSingleHitCallback(
		DEF_EVENT_HIT_GUNNER_SLIDING, CC_CALLBACK_1(GunnerSliding::OnEnemySingleHit, this));
	pHitRecorder_->AddMultiHitCallback(
		DEF_EVENT_HIT_GUNNER_SLIDING, CC_CALLBACK_2(GunnerSliding::OnEnemyMultiHit, this));
}

void GunnerSliding::OnUpdate(float _deltaTime)
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (!slidingStarted_)
	{
		return;
	}

	if (pPhysicsComponent == nullptr)
	{
		Stop();
		return;
	}

	if (pPhysicsComponent->HasForceX())
	{
		return;
	}

	// 더이상 X축 힘이 존재하지 않는 경우 중지
	Stop();
}

void GunnerSliding::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();
	const SpriteDirection_t spriteDirection = pPlayer_->GetSpriteDirection();

	// 거너 특유의 슬라이딩 시작전 경직 효과를 주기위해 111번 프레임 끝난 후
	if (_pAnimation->GetFrameIndex() == 111)
	{
		slidingStarted_ = true;

		ActorManager::Get()->CreateEffectOnMapBySpawner(pPlayer_, DEF_EFFECT_GUNNER_SLIDING_BEGIN, 250, 140);

		if (pPhysicsComponent == nullptr)
		{
			return;
		}

		if (spriteDirection == SpriteDirection::Right)
		{
			pPhysicsComponent->AddForceX(pPlayer_->GetBaseInfo()->slidingForce_);
		}
		else
		{
			pPhysicsComponent->AddForceX(-pPlayer_->GetBaseInfo()->slidingForce_);
		}
	}

	// 일시정지 프레임 만나면 1번부터 다시 재시작
	if (_pAnimation->GetFrameIndex() == 114 && _pAnimation->IsZeroFramePaused())
	{
		pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_SLIDING, 1);
	}
}

void GunnerSliding::OnEnemySingleHit(HitInfo& _info)
{
	if (pHitRecorder_->IsAlreadyHit(_info.pHitTarget_))
	{
		return;
	}

	ActorManager::Get()->CreateEffectOnMapTargetCollision(DEF_EFFECT_KNOCK_BIG, _info, true);
	_info.pHitTarget_->Hit(_info);
}

void GunnerSliding::OnEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount)
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent == nullptr)
	{
		return;
	}

	if (_newHitCount > 0)
	{
		pPhysicsComponent->StiffenBody(Const::FPS::_6);
	}
}
