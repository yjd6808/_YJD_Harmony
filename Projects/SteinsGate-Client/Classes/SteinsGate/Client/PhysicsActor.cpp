/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 7:00:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "PhysicsActor.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/MapLayer.h>
#include <SteinsGate/Client/Global.h>

#include <SteinsGate/Common/RectPoly.h>

USING_NS_CC;
USING_NS_JC;

PhysicsActor::PhysicsActor(ActorType_t type, int code)
	: Actor(type, code)
	, m_bUseElasticity(false)
	, m_bBounced(false)
	, m_fWeight(0.0f)
	, m_fUpTime(0.0f)
	, m_fDownTime(0.0f)
	, m_fVelocityX(0.0f)
	, m_fVelocityY(0.0f)
	, m_fPuaseTime(0.0f)
	, m_fElapsedPausedTime(0.0f)
	, m_bDead(false)
	, m_fAtkBoxInstantElapsedTime(0.0f)
	, m_pAtkThicknessBox(nullptr)
	, m_pAtkHitBox(nullptr)
{}

bool PhysicsActor::initVariables() {
	Actor::initVariables();

	m_bUseElasticity = false;
	m_bBounced = false;
	m_fUpTime = 0.0f;
	m_fDownTime = 0.0f;
	m_fVelocityX = 0.0f;
	m_fVelocityY = 0.0f;
	m_fPuaseTime = 0.0f;
	m_fElapsedPausedTime = 0.0f;
	m_bDead = false;
	m_fAtkBoxInstantElapsedTime = 0.0f;

	return true;
}

bool PhysicsActor::isPaused() {
	return m_fElapsedPausedTime < m_fPuaseTime;
}

bool PhysicsActor::isBounced() {
	return m_bBounced;
}

bool PhysicsActor::isDead() {
	return m_bDead;
}

void PhysicsActor::disableElasticity() {
	m_bUseElasticity = false;
}

void PhysicsActor::enableElasticity() {
	m_bUseElasticity = true;
}


void PhysicsActor::update(float dt) {

	if (m_bDead)
		return;

	Actor::update(dt);
	updatePauseTime(dt);
	updatePhysics(dt);
	updateDebug(dt);		// TODO: 디버깅용 임시 코드
}

void PhysicsActor::updatePauseTime(float dt) {
	m_fElapsedPausedTime += dt;

	if (m_fElapsedPausedTime >= m_fPuaseTime) {
		m_fPuaseTime = 0.0f;
		m_fElapsedPausedTime = 0.0f;
	}
}

void PhysicsActor::updatePhysics(float dt) {

	if (isPaused())
		return;

	updateGravity(dt);
	updateFriction(dt);
}

void PhysicsActor::updateGravity(float dt) {
	const MapPhysicsInfo* pPhysicsInfo = m_pMapLayer->getMapPhysicsInfo();

	float y = m_pActorSprite->getPositionY() + m_fVelocityY * FPS1_v;
	m_fVelocityY -= pPhysicsInfo->Gravity * FPS1_v;

	if (y > SG_FLT_EPSILON && m_fVelocityY > 0.0f) {
		m_fUpTime += dt;
		m_fDownTime = 0.0f;
	}

	if (y > SG_FLT_EPSILON && m_fVelocityY < 0.0f) {
		m_fUpTime = 0.0f;
		m_fDownTime += dt;
	}

	if (y <= SG_FLT_EPSILON) {

		if (m_bUseElasticity && !m_bBounced && m_fDownTime > 0.0f) {
			m_fVelocityY = Math::Abs(m_fVelocityY / pPhysicsInfo->ElasticityDividedForce);
			m_bBounced = true;

		} else {
			m_fVelocityY = 0.0f;
			m_bBounced = false;
		}

		m_fDownTime = 0.0f;
		y = 0.0f;
	}

	m_pActorSprite->setPositionY(y);
}

void PhysicsActor::updateFriction(float dt) {
	MapAreaInfo* pAreaInfo = m_pMapLayer->getMapAreaInfo();
	MapPhysicsInfo* pPhysicsInfo = m_pMapLayer->getMapPhysicsInfo();
	SGRect groundRect = getThicknessBoxRect();

	groundRect.origin.x += m_fVelocityX * FPS1_v;

	if (isOnTheGround()) {

		if (m_fVelocityX > 0.0f) {
			m_fVelocityX -= pPhysicsInfo->Friction * FPS1_v;

			if (m_fVelocityX <= 0.0f) {
				m_fVelocityX = 0.0f;
			}
		}
		
		else if (m_fVelocityX < 0.0f) { 
			m_fVelocityX += pPhysicsInfo->Friction * FPS1_v;

			if (m_fVelocityX >= 0.0f) {
				m_fVelocityX = 0.0f;
			}
		}
	}

	if (pAreaInfo->checkWall(groundRect.origin.x, groundRect.origin.y)) {
		return;
	}

	if (m_pMapLayer->isCollideWithMapObjects(groundRect))
		return;

	setPositionRealX(groundRect.origin.x);
}




float PhysicsActor::addForceX(float force) {
	float fBefore = m_fVelocityX;
	m_fVelocityX += force;
	return fBefore;
}

float PhysicsActor::addForceY(float force) {
	float fBefore = m_fVelocityY;
	m_fVelocityY += force;
	return fBefore;
}

float PhysicsActor::removeForceX() {
	float fBefore = m_fVelocityX;
	m_fVelocityX = 0.0f;
	return fBefore;
}

float PhysicsActor::removeForceY() {
	float fBefore = m_fVelocityY;
	m_fVelocityY = 0.0f;
	return fBefore;
}

bool PhysicsActor::hasForceX() {
	return Math::Abs(m_fVelocityX) >= SG_FLT_EPSILON;
}

bool PhysicsActor::hasForceY() {
	return Math::Abs(m_fVelocityY) >= SG_FLT_EPSILON;
}



void PhysicsActor::hit(const HitInfo& hitInfo) {
	hit(hitInfo.Attacker, hitInfo.HitDirection, hitInfo.HitRect, hitInfo.AttackDataInfo);
}

void PhysicsActor::hit(
	Actor* attacker,
	const SpriteDirection_t hitDirection, 
	const SGRect& hitRect,
	AttackDataInfo* attackDataInfo)
{
	float fForceX = 500;

	switch (attackDataInfo->AttackXForceDir) {
	case AttackXForceDirection::Forward:
		fForceX = attacker->getSpriteDirection() == SpriteDirection::Right
			? attackDataInfo->AttackXForce
			: -attackDataInfo->AttackXForce;
		break;
	case AttackXForceDirection::Backward:
		fForceX = attacker->getSpriteDirection() == SpriteDirection::Right
			? -attackDataInfo->AttackXForce
			: attackDataInfo->AttackXForce;
		break;
	case AttackXForceDirection::Spread:
	case AttackXForceDirection::None:
		fForceX = hitDirection == SpriteDirection::Right
			? -attackDataInfo->AttackXForce
			: attackDataInfo->AttackXForce;
		break;
	default: DebugAssertMsg(false, "뭐야! 이상한 타입의 X 포스 디렉션입니다."); break;
	}

	m_bBounced = false;

	setSpriteDirection(hitDirection);

	int iRemovedForceX = removeForceX();
	int iRemovedForceY = removeForceY();
	
	if (attackDataInfo->IsFallDownAttack) {
		addForceX(fForceX);
		addForceY(attackDataInfo->AttackYForce);
		return;
	}

	if (iRemovedForceX != 0) {
		addForceY(attackDataInfo->AttackYForce);
		return;
	}

	if (isOnTheGround()) {
		addForceX(fForceX);
		return;
	}

	addForceY(attackDataInfo->AttackYForce);
}

void PhysicsActor::dead() {
	m_bDead = true;

	removeForceX();
	removeForceY();
}

void PhysicsActor::pausePhysics() {
	pausePhysics(FLT_MAX);
}

void PhysicsActor::pausePhysics(float time) {
	m_fPuaseTime = time;
}

void PhysicsActor::stiffenBody(float time) {
	pausePhysics(time);
	pauseAnimation(time);
}

void PhysicsActor::resume() {
	m_fPuaseTime = 0.0f;
}

Direction_t PhysicsActor::getForceXDirection() {

	if (m_fVelocityX > SG_FLT_EPSILON)
		return Direction::Right;

	if (m_fVelocityX < -SG_FLT_EPSILON)
		return Direction::Left;


	return Direction::None;
}

Direction_t PhysicsActor::getForceYDirection() {

	if (m_fVelocityY > SG_FLT_EPSILON)
		return Direction::Up;

	if (m_fVelocityY < -SG_FLT_EPSILON)
		return Direction::Down;

	return Direction::None;
}

bool PhysicsActor::hasForce() {

	if (Math::Abs(m_fVelocityX) >= SG_FLT_EPSILON)
		return true;

	if (Math::Abs(m_fVelocityY) >= SG_FLT_EPSILON)
		return true;

	return false;
}


// 디버깅용으로 임시로 추가한 코드
// 어택박스 위치가 제대로 맞나 눈으로 확인하기 위한 용도
void PhysicsActor::updateDebug(float dt) {
	if (!Global::Get()->DrawAttackBox) {
		if (m_pAtkThicknessBox) m_pAtkThicknessBox->setOpacity(0);
		if (m_pAtkHitBox) m_pAtkHitBox->setOpacity(0);
		return;
	}

	updateDebugSub1(dt);
	updateDebugSub2(dt);
}


void PhysicsActor::updateDebugSub1(float dt) {
	ActorPartAnimation* pAnimation = m_pActorSprite->getRunningAnimation();

	if (pAnimation == nullptr)
		return;

	const FrameInfo& frameInfo = pAnimation->getRunningFrameInfo();
	const FrameEvent* pFrameEvent = CoreDataManager_v->getFrameEvent(m_eActorType, frameInfo.FrameEventCode);

	if (pFrameEvent && pFrameEvent->Type != FrameEventType::AttackBoxInstant) {
		return;
	}

	// 어택박스 프레임 발견할때마다 초기화
	m_fAtkBoxInstantElapsedTime = 0.0f;

	FrameEventAttackBoxInstant* pAttackBoxInstantInfo = (FrameEventAttackBoxInstant*)pFrameEvent;
	ActorRect absoluteActorRect = convertAbsoluteActorRect(this, pAttackBoxInstantInfo->Rect);

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
		m_pAtkHitBox->drawPolygon(hitPoly.source(), 4, {}, 1.0, Color4F{Color3B::YELLOW, 0.7f});
		m_pMapLayer->addChild(m_pAtkHitBox, 1000);
	} else {
		m_pAtkHitBox->clear();
		m_pAtkHitBox->setOpacity(255);
		m_pAtkHitBox->drawPolygon(hitPoly.source(), 4, {}, 1.0, Color4F{ Color3B::YELLOW, 0.7f });
	}

	if (m_pAtkThicknessBox == nullptr) {
		m_pAtkThicknessBox = DrawNode::create();
		m_pAtkThicknessBox->drawPolygon(thickPoly.source(), 4, {}, 1.0, Color4F{ Color3B::YELLOW, 0.7f });
		m_pMapLayer->addChild(m_pAtkThicknessBox, 1000);
	} else {
		m_pAtkThicknessBox->clear();
		m_pAtkThicknessBox->setOpacity(255);
		m_pAtkThicknessBox->drawPolygon(thickPoly.source(), 4, {}, 1.0, Color4F{ Color3B::YELLOW, 0.7f });
	}
}

void PhysicsActor::updateDebugSub2(float dt) {
	m_fAtkBoxInstantElapsedTime += dt;

	if (m_fAtkBoxInstantElapsedTime >= 1.0f) {
		if (m_pAtkThicknessBox) m_pAtkThicknessBox->setOpacity(0);
		if (m_pAtkHitBox) m_pAtkHitBox->setOpacity(0);
	}
}
