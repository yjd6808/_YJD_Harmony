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
#include <SteinsGate/Client/ActorBox.h>
#include <SteinsGate/Client/PhysicsComponent.h>

GunnerSliding::GunnerSliding(HostPlayer* player, ActionInfo* actionInfo)
	: GunnerAction(player, actionInfo)
{
}

bool GunnerSliding::onConditionCheck() {
	return m_pPlayer->getRunningActionCode() == DEF_ACTION_GUNNER_RUN;
}

void GunnerSliding::onActionBegin() {
	m_bSlidingStarted = false;
	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_SLIDING);
	m_pHitRecorder->setAlreadyHitRecord(true);
	m_pHitRecorder->addSingleHitCallback(DEF_EVENT_HIT_GUNNER_SLIDING, CC_CALLBACK_1(GunnerSliding::onEnemySingleHit, this));
	m_pHitRecorder->addMultiHitCallback(DEF_EVENT_HIT_GUNNER_SLIDING, CC_CALLBACK_2(GunnerSliding::onEnemyMultiHit, this));
	
}

void GunnerSliding::onUpdate(float dt) {

	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (!m_bSlidingStarted)
		return;

	if (!pPhysicsComponent) {
		stop();
		return;
	}

	if (pPhysicsComponent->hasForceX())
		return;

	// 더이상 X축 힘이 존재하지 않는 경우 중지
	stop();
}


void GunnerSliding::onFrameEnd(ActorPartAnimation* animation, FrameTexture* frame) {
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();
	const SpriteDirection_t eDir = m_pPlayer->getSpriteDirection();

	// 거너 특유의 슬라이딩 시작전 경직 효과를 주기위해 111번 프레임 끝난 후
	if (animation->getFrameIndex() == 111) {
		m_bSlidingStarted = true;

		ActorBox::Get()->createEffectOnMapBySpawner(m_pPlayer, DEF_EFFECT_GUNNER_SLIDING_BEGIN, 250, 140);

		if (pPhysicsComponent == nullptr) {
			return;
		}

		if (eDir == SpriteDirection::Right) 
			pPhysicsComponent->addForceX(m_pPlayer->getBaseInfo()->SlidingForce);
		else
			pPhysicsComponent->addForceX(-m_pPlayer->getBaseInfo()->SlidingForce);
	}

	// 일시정지 프레임 만나면 1번부터 다시 재시작
	if (animation->getFrameIndex() == 114 && animation->isZeroFramePaused()) {
		m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_SLIDING, 1);
	}
}

void GunnerSliding::onEnemySingleHit(HitInfo& info) {
	if (m_pHitRecorder->isAlreadyHit(info.HitTarget))
		return;

	ActorBox::Get()->createEffectOnMapTargetCollision(DEF_EFFECT_KNOCK_BIG, info, true);
	info.HitTarget->hit(info);
}


void GunnerSliding::onEnemyMultiHit(SGHitInfoList& hitList, int newHitCount) {
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent == nullptr)
		return;

	if (newHitCount > 0) {
		pPhysicsComponent->stiffenBody(Const::FPS::_6);
	}
}