/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerSliding.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Action.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/Define_Effect.h>
#include <SteinsGate/Client/Define_Event.h>
#include <SteinsGate/Client/ActorManager.h>
#include <SteinsGate/Client/PhysicsComponent.h>

//////////////////////////////////////////////////////////////////////////////////////////
GunnerSliding::GunnerSliding(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

bool GunnerSliding::onConditionCheck()
{
	return m_pPlayer->getRunningActionCode() == DEF_ACTION_GUNNER_RUN;
}

void GunnerSliding::onActionBegin()
{
	slidingStarted_ = false;
	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_SLIDING);
	m_pHitRecorder->setAlreadyHitRecord(true);
	m_pHitRecorder->addSingleHitCallback(
		DEF_EVENT_HIT_GUNNER_SLIDING, CC_CALLBACK_1(GunnerSliding::onEnemySingleHit, this));
	m_pHitRecorder->addMultiHitCallback(
		DEF_EVENT_HIT_GUNNER_SLIDING, CC_CALLBACK_2(GunnerSliding::onEnemyMultiHit, this));
}

void GunnerSliding::onUpdate(float _deltaTime)
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (!slidingStarted_)
	{
		return;
	}

	if (pPhysicsComponent == nullptr)
	{
		stop();
		return;
	}

	if (pPhysicsComponent->hasForceX())
	{
		return;
	}

	// 더이상 X축 힘이 존재하지 않는 경우 중지
	stop();
}

void GunnerSliding::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();
	const SpriteDirection_t spriteDirection = m_pPlayer->getSpriteDirection();

	// 거너 특유의 슬라이딩 시작전 경직 효과를 주기위해 111번 프레임 끝난 후
	if (_pAnimation->getFrameIndex() == 111)
	{
		slidingStarted_ = true;

		ActorManager::Get()->createEffectOnMapBySpawner(m_pPlayer, DEF_EFFECT_GUNNER_SLIDING_BEGIN, 250, 140);

		if (pPhysicsComponent == nullptr)
		{
			return;
		}

		if (spriteDirection == SpriteDirection::Right)
		{
			pPhysicsComponent->addForceX(m_pPlayer->getBaseInfo()->slidingForce_);
		}
		else
		{
			pPhysicsComponent->addForceX(-m_pPlayer->getBaseInfo()->slidingForce_);
		}
	}

	// 일시정지 프레임 만나면 1번부터 다시 재시작
	if (_pAnimation->getFrameIndex() == 114 && _pAnimation->isZeroFramePaused())
	{
		m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_SLIDING, 1);
	}
}

void GunnerSliding::onEnemySingleHit(HitInfo& _info)
{
	if (m_pHitRecorder->isAlreadyHit(_info.HitTarget))
	{
		return;
	}

	ActorManager::Get()->createEffectOnMapTargetCollision(DEF_EFFECT_KNOCK_BIG, _info, true);
	_info.HitTarget->hit(_info);
}

void GunnerSliding::onEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount)
{
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent == nullptr)
	{
		return;
	}

	if (_newHitCount > 0)
	{
		pPhysicsComponent->stiffenBody(Const::FPS::_6);
	}
}
