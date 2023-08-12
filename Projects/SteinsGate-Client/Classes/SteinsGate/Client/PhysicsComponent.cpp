/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:06:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "PhysicsComponent.h"

#include <SteinsGate/Common/RectPoly.h>

#include <SteinsGate/Client/MapLayer.h>

USING_NS_JC;
USING_NS_CC;


PhysicsComponent::PhysicsComponent(Actor* actor)
	: ActorComponent(actor)
	, m_bUseElasticity(false)
	, m_bBounced(false)
	, m_fWeight(0.0f)
	, m_fUpTime(0.0f)
	, m_fDownTime(0.0f)
	, m_fElapsedPausedTime(0.0f)
	, m_fPuaseTime(0.0f)
	, m_fAtkBoxInstantElapsedTime(0.0f)
	, m_pAtkThicknessBox(nullptr)
	, m_pAtkHitBox(nullptr)
{}

void PhysicsComponent::initialize() {
	m_bUseElasticity = false;
	m_bBounced = false;
	m_fUpTime = 0.0f;
	m_fDownTime = 0.0f;
	m_Velocity.x = 0.0f;
	m_Velocity.y = 0.0f;
	m_fPuaseTime = 0.0f;
	m_fElapsedPausedTime = 0.0f;
	m_fAtkBoxInstantElapsedTime = 0.0f;
}

bool PhysicsComponent::isPaused() {
	return m_fElapsedPausedTime < m_fPuaseTime;
}

bool PhysicsComponent::isBounced() {
	return m_bBounced;
}

void PhysicsComponent::disableElasticity() {
	m_bUseElasticity = false;
}

void PhysicsComponent::enableElasticity() {
	m_bUseElasticity = true;
}


void PhysicsComponent::onUpdate(float dt) {
	updatePauseTime(dt);
	updatePhysics(dt);
	updateDebug(dt);		// TODO: 디버깅용 임시 코드
}

void PhysicsComponent::updatePauseTime(float dt) {
	m_fElapsedPausedTime += dt;

	if (m_fElapsedPausedTime >= m_fPuaseTime) {
		m_fPuaseTime = 0.0f;
		m_fElapsedPausedTime = 0.0f;
	}
}

void PhysicsComponent::updatePhysics(float dt) {

	if (isPaused())
		return;

	updateGravity(dt);
	updateFriction(dt);
}

void PhysicsComponent::updateGravity(float dt) {
	const MapLayer* pMapLayer = m_pActor->getMapLayer();

	if (pMapLayer == nullptr)
		return;

	const MapPhysicsInfo* pPhysicsInfo = pMapLayer->getMapPhysicsInfo();

	if (pPhysicsInfo == nullptr)
		return;

	ActorSprite* pActorSprite = m_pActor->getActorSprite();

	if (pActorSprite == nullptr)
		return;

	float y = pActorSprite->getPositionY() + m_Velocity.y * dt;
	m_Velocity.y -= pPhysicsInfo->Gravity * dt;

	if (y > SG_FLT_EPSILON && m_Velocity.y > 0.0f) {
		m_fUpTime += dt;
		m_fDownTime = 0.0f;
	}

	if (y > SG_FLT_EPSILON && m_Velocity.y < 0.0f) {
		m_fUpTime = 0.0f;
		m_fDownTime += dt;
	}

	if (y <= SG_FLT_EPSILON) {

		if (m_bUseElasticity && !m_bBounced && m_fDownTime > 0.0f) {
			m_Velocity.y = Math::Abs(m_Velocity.y / pPhysicsInfo->ElasticityDividedForce);
			m_bBounced = true;

		} else {
			m_Velocity.y = 0.0f;
			m_bBounced = false;
		}

		m_fDownTime = 0.0f;
		y = 0.0f;
	}

	pActorSprite->setPositionY(y);
}

void PhysicsComponent::updateFriction(float dt) {
	const MapLayer* pMapLayer = m_pActor->getMapLayer();

	if (pMapLayer == nullptr)
		return;

	const MapPhysicsInfo* pPhysicsInfo = pMapLayer->getMapPhysicsInfo();
	const MapAreaInfo* pAreaInfo = pMapLayer->getMapAreaInfo();

	if (pPhysicsInfo == nullptr || pAreaInfo == nullptr)
		return;
	

	SGRect groundRect = m_pActor->getThicknessBoxRect();
	groundRect.origin.x += m_Velocity.x * FPS1_v;

	if (m_pActor->isOnTheGround()) {

		if (m_Velocity.x > 0.0f) {
			m_Velocity.x -= pPhysicsInfo->Friction * FPS1_v;

			if (m_Velocity.x <= 0.0f) {
				m_Velocity.x = 0.0f;
			}
		}

		else if (m_Velocity.x < 0.0f) {
			m_Velocity.x += pPhysicsInfo->Friction * FPS1_v;

			if (m_Velocity.x >= 0.0f) {
				m_Velocity.x = 0.0f;
			}
		}
	}

	if (pAreaInfo->checkWall(groundRect.origin.x, groundRect.origin.y)) {
		return;
	}

	if (pMapLayer->isCollideWithMapObjects(groundRect))
		return;


	m_pActor->setPositionRealX(groundRect.origin.x);
}




float PhysicsComponent::addForceX(float force) {
	float fBefore = m_Velocity.x;
	m_Velocity.x += force;
	return fBefore;
}

float PhysicsComponent::addForceY(float force) {
	float fBefore = m_Velocity.y;
	m_Velocity.y += force;
	return fBefore;
}

float PhysicsComponent::removeForceX() {
	float fBefore = m_Velocity.x;
	m_Velocity.x = 0.0f;
	return fBefore;
}

float PhysicsComponent::removeForceY() {
	float fBefore = m_Velocity.y;
	m_Velocity.y = 0.0f;
	return fBefore;
}

bool PhysicsComponent::hasForceX() {
	return Math::Abs(m_Velocity.x) >= SG_FLT_EPSILON;
}

bool PhysicsComponent::hasForceY() {
	return Math::Abs(m_Velocity.y) >= SG_FLT_EPSILON;
}

void PhysicsComponent::hit(const HitInfo& info) {

	const Actor* pAttacker = info.Attacker;
	const SpriteDirection_t eHitDirection = info.HitDirection;
	const SGRect& hitRect = info.HitRect;
	const AttackDataInfo* pAttackDataInfo = info.AttackDataInfo;

	float fForceX = 500;

	switch (pAttackDataInfo->AttackXForceDir) {
	case AttackXForceDirection::Forward:
		fForceX = pAttacker->getSpriteDirection() == SpriteDirection::Right
			? pAttackDataInfo->AttackXForce
			: -pAttackDataInfo->AttackXForce;
		break;
	case AttackXForceDirection::Backward:
		fForceX = pAttacker->getSpriteDirection() == SpriteDirection::Right
			? -pAttackDataInfo->AttackXForce
			: pAttackDataInfo->AttackXForce;
		break;
	case AttackXForceDirection::Spread:
	case AttackXForceDirection::None:
		fForceX = eHitDirection == SpriteDirection::Right
			? -pAttackDataInfo->AttackXForce
			: pAttackDataInfo->AttackXForce;
		break;
	default: DebugAssertMsg(false, "뭐야! 이상한 타입의 X 포스 디렉션입니다."); break;
	}

	m_bBounced = false;
	m_pActor->setSpriteDirection(eHitDirection);

	int iRemovedForceX = removeForceX();
	int iRemovedForceY = removeForceY();

	if (pAttackDataInfo->IsFallDownAttack) {
		addForceX(fForceX);
		addForceY(pAttackDataInfo->AttackYForce);
		return;
	}

	if (iRemovedForceX != 0) {
		addForceY(pAttackDataInfo->AttackYForce);
		return;
	}

	if (m_pActor->isOnTheGround()) {
		addForceX(fForceX);
		return;
	}

	addForceY(pAttackDataInfo->AttackYForce);
}

void PhysicsComponent::pausePhysics() {
	pausePhysics(FLT_MAX);
}

void PhysicsComponent::pausePhysics(float time) {
	m_fPuaseTime = time;
}

void PhysicsComponent::stiffenBody(float time) {
	pausePhysics(time);
	m_pActor->pauseAnimation(time);
}

void PhysicsComponent::resume() {
	m_fPuaseTime = 0.0f;
}

Direction_t PhysicsComponent::getForceXDirection() {

	if (m_Velocity.x > SG_FLT_EPSILON)
		return Direction::Right;

	if (m_Velocity.x < -SG_FLT_EPSILON)
		return Direction::Left;


	return Direction::None;
}

Direction_t PhysicsComponent::getForceYDirection() {

	if (m_Velocity.y > SG_FLT_EPSILON)
		return Direction::Up;

	if (m_Velocity.y < -SG_FLT_EPSILON)
		return Direction::Down;

	return Direction::None;
}

bool PhysicsComponent::hasForce() {

	if (Math::Abs(m_Velocity.x) >= SG_FLT_EPSILON)
		return true;

	if (Math::Abs(m_Velocity.y) >= SG_FLT_EPSILON)
		return true;

	return false;
}


// 디버깅용으로 임시로 추가한 코드
// 어택박스 위치가 제대로 맞나 눈으로 확인하기 위한 용도
void PhysicsComponent::updateDebug(float dt) {
	if (!Global::Get()->DrawAttackBox) {
		if (m_pAtkThicknessBox) m_pAtkThicknessBox->setOpacity(0);
		if (m_pAtkHitBox) m_pAtkHitBox->setOpacity(0);
		return;
	}

	updateDebugSub1(dt);
	updateDebugSub2(dt);
}


void PhysicsComponent::updateDebugSub1(float dt) {
	MapLayer* pMapLayer = m_pActor->getMapLayer();
	const ActorSprite* pActorSprite = m_pActor->getActorSprite();

	if (pMapLayer == nullptr)
		return;

	ActorPartAnimation* pAnimation = pActorSprite->getRunningAnimation();

	if (pAnimation == nullptr)
		return;

	const FrameInfo& frameInfo = pAnimation->getRunningFrameInfo();

	if (frameInfo.FrameEventCode == InvalidValue_v)
		return;

	const FrameEvent* pFrameEvent = CoreDataManager_v->getFrameEvent(m_pActor->getType(), frameInfo.FrameEventCode);

	if (pFrameEvent && pFrameEvent->Type != FrameEventType::AttackBoxInstant) {
		return;
	}

	// 어택박스 프레임 발견할때마다 초기화
	m_fAtkBoxInstantElapsedTime = 0.0f;

	FrameEventAttackBoxInstant* pAttackBoxInstantInfo = (FrameEventAttackBoxInstant*)pFrameEvent;
	ActorRect absoluteActorRect = Actor::convertAbsoluteActorRect(m_pActor, pAttackBoxInstantInfo->Rect);

	RectPoly hitPoly = RectPoly::createFromLeftBottom(
		absoluteActorRect.BodyRect.origin,
		absoluteActorRect.BodyRect.size
	);

	RectPoly thickPoly = RectPoly::createFromLeftBottom(
		absoluteActorRect.ThicknessRect.origin,
		absoluteActorRect.ThicknessRect.size
	);

	if (m_pAtkHitBox == nullptr) {
		m_pAtkHitBox = DrawNode::create();
		m_pAtkHitBox->drawPolygon(hitPoly.source(), 4, {}, 1.0, Color4F{ Color3B::YELLOW, 0.7f });
		pMapLayer->addChild(m_pAtkHitBox, 1000);
	}
	else {
		m_pAtkHitBox->clear();
		m_pAtkHitBox->setOpacity(255);
		m_pAtkHitBox->drawPolygon(hitPoly.source(), 4, {}, 1.0, Color4F{ Color3B::YELLOW, 0.7f });
	}

	if (m_pAtkThicknessBox == nullptr) {
		m_pAtkThicknessBox = DrawNode::create();
		m_pAtkThicknessBox->drawPolygon(thickPoly.source(), 4, {}, 1.0, Color4F{ Color3B::YELLOW, 0.7f });
		pMapLayer->addChild(m_pAtkThicknessBox, 1000);
	}
	else {
		m_pAtkThicknessBox->clear();
		m_pAtkThicknessBox->setOpacity(255);
		m_pAtkThicknessBox->drawPolygon(thickPoly.source(), 4, {}, 1.0, Color4F{ Color3B::YELLOW, 0.7f });
	}
}

void PhysicsComponent::updateDebugSub2(float dt) {
	m_fAtkBoxInstantElapsedTime += dt;

	if (m_fAtkBoxInstantElapsedTime >= 1.0f) {
		if (m_pAtkThicknessBox) m_pAtkThicknessBox->setOpacity(0);
		if (m_pAtkHitBox) m_pAtkHitBox->setOpacity(0);
	}
}

