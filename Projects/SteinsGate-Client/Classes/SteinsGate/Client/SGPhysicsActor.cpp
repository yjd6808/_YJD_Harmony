/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 7:00:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGPhysicsActor.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGMapLayer.h>
#include <SteinsGate/Client/SGGlobal.h>

#include <SteinsGate/Common/RectPoly.h>

USING_NS_CC;
USING_NS_JC;

SGPhysicsActor::SGPhysicsActor(ActorType_t type, int code)
	: SGActor(type, code)
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

bool SGPhysicsActor::initVariables() {
	SGActor::initVariables();

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

bool SGPhysicsActor::isPaused() {
	return m_fElapsedPausedTime < m_fPuaseTime;
}

bool SGPhysicsActor::isBounced() {
	return m_bBounced;
}

bool SGPhysicsActor::isDead() {
	return m_bDead;
}

void SGPhysicsActor::disableElasticity() {
	m_bUseElasticity = false;
}

void SGPhysicsActor::enableElasticity() {
	m_bUseElasticity = true;
}


void SGPhysicsActor::update(float dt) {

	if (m_bDead)
		return;

	SGActor::update(dt);

	updatePauseTime(dt);
	updatePhysics(dt);
	updateDebug(dt);		// TODO: 디버깅용 임시 코드
}

void SGPhysicsActor::updatePauseTime(float dt) {
	m_fElapsedPausedTime += dt;

	if (m_fElapsedPausedTime >= m_fPuaseTime) {
		m_fPuaseTime = 0.0f;
		m_fElapsedPausedTime = 0.0f;
	}
}

void SGPhysicsActor::updatePhysics(float dt) {

	if (isPaused())
		return;

	updateGravity(dt);
	updateFriction(dt);
}

void SGPhysicsActor::updateGravity(float dt) {
	SGMapInfo* pMapInfo = m_pMapLayer->getMapInfo();

	float y = m_pActorSprite->getPositionY() + m_fVelocityY * FPS1_v;
	m_fVelocityY -= pMapInfo->Gravity * FPS1_v;

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
			m_fVelocityY = Math::Abs(m_fVelocityY / pMapInfo->ElasticityDividedForce);
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

void SGPhysicsActor::updateFriction(float dt) {
	SGMapInfo* pMapInfo = m_pMapLayer->getMapInfo();
	SGRect groundRect = getThicknessBoxRect();

	groundRect.origin.x += m_fVelocityX * FPS1_v;

	if (isOnTheGround()) {

		if (m_fVelocityX > 0.0f) {
			m_fVelocityX -= pMapInfo->Friction * FPS1_v;

			if (m_fVelocityX <= 0.0f) {
				m_fVelocityX = 0.0f;
			}
		}
		
		else if (m_fVelocityX < 0.0f) { 
			m_fVelocityX += pMapInfo->Friction * FPS1_v;

			if (m_fVelocityX >= 0.0f) {
				m_fVelocityX = 0.0f;
			}
		}
	}

	if (pMapInfo->checkWall(groundRect.origin.x, groundRect.origin.y)) {
		return;
	}

	if (m_pMapLayer->isCollideWithObstacles(groundRect))
		return;

	setPositionRealX(groundRect.origin.x);
}




float SGPhysicsActor::addForceX(float force) {
	float fBefore = m_fVelocityX;
	m_fVelocityX += force;
	return fBefore;
}

float SGPhysicsActor::addForceY(float force) {
	float fBefore = m_fVelocityY;
	m_fVelocityY += force;
	return fBefore;
}

float SGPhysicsActor::removeForceX() {
	float fBefore = m_fVelocityX;
	m_fVelocityX = 0.0f;
	return fBefore;
}

float SGPhysicsActor::removeForceY() {
	float fBefore = m_fVelocityY;
	m_fVelocityY = 0.0f;
	return fBefore;
}

bool SGPhysicsActor::hasForceX() {
	return Math::Abs(m_fVelocityX) >= SG_FLT_EPSILON;
}

bool SGPhysicsActor::hasForceY() {
	return Math::Abs(m_fVelocityY) >= SG_FLT_EPSILON;
}



void SGPhysicsActor::hit(const SGHitInfo& hitInfo) {
	hit(hitInfo.Attacker, hitInfo.HitDirection, hitInfo.HitRect, hitInfo.AttackDataInfo);
}

void SGPhysicsActor::hit(
	SGActor* attacker,
	const SpriteDirection_t hitDirection, 
	const SGRect& hitRect,
	SGAttackDataInfo* attackDataInfo)
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

void SGPhysicsActor::dead() {
	m_bDead = true;

	removeForceX();
	removeForceY();
}

void SGPhysicsActor::pausePhysics() {
	pausePhysics(FLT_MAX);
}

void SGPhysicsActor::pausePhysics(float time) {
	m_fPuaseTime = time;
}

void SGPhysicsActor::stiffenBody(float time) {
	pausePhysics(time);
	pauseAnimation(time);
}

void SGPhysicsActor::resume() {
	m_fPuaseTime = 0.0f;
}

Direction_t SGPhysicsActor::getForceXDirection() {

	if (m_fVelocityX > SG_FLT_EPSILON)
		return Direction::Right;

	if (m_fVelocityX < -SG_FLT_EPSILON)
		return Direction::Left;


	return Direction::None;
}

Direction_t SGPhysicsActor::getForceYDirection() {

	if (m_fVelocityY > SG_FLT_EPSILON)
		return Direction::Up;

	if (m_fVelocityY < -SG_FLT_EPSILON)
		return Direction::Down;

	return Direction::None;
}

bool SGPhysicsActor::hasForce() {

	if (Math::Abs(m_fVelocityX) >= SG_FLT_EPSILON)
		return true;

	if (Math::Abs(m_fVelocityY) >= SG_FLT_EPSILON)
		return true;

	return false;
}


// 디버깅용으로 임시로 추가한 코드
// 어택박스 위치가 제대로 맞나 눈으로 확인하기 위한 용도
void SGPhysicsActor::updateDebug(float dt) {

	if (!SGGlobal::get()->isAttackBoxDrawMode()) {
		if (m_pAtkThicknessBox) m_pAtkThicknessBox->setOpacity(0);
		if (m_pAtkHitBox) m_pAtkHitBox->setOpacity(0);
		return;
	}

	updateDebugSub1(dt);
	updateDebugSub2(dt);
}


void SGPhysicsActor::updateDebugSub1(float dt) {
	SGActorPartAnimation* pAnimation = m_pActorSprite->getRunningAnimation();

	if (pAnimation == nullptr)
		return;

	SGFrameInfo* pFrameInfo = pAnimation->getRunningFrameInfo();

	if (pFrameInfo->FrameEvent != FrameEventType::AttackBoxInstant) {
		return;
	}

	// 어택박스 프레임 발견할때마다 초기화
	m_fAtkBoxInstantElapsedTime = 0.0f;

	SGFrameInfoAttackBoxInstant* pAttackBoxInstantInfo = (SGFrameInfoAttackBoxInstant*)pFrameInfo;
	SGActorRect absoluteActorRect = SGActor::convertAbsoluteActorRect(this, pAttackBoxInstantInfo->Rect);

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

void SGPhysicsActor::updateDebugSub2(float dt) {
	m_fAtkBoxInstantElapsedTime += dt;

	if (m_fAtkBoxInstantElapsedTime >= 1.0f) {
		if (m_pAtkThicknessBox) m_pAtkThicknessBox->setOpacity(0);
		if (m_pAtkHitBox) m_pAtkHitBox->setOpacity(0);
	}
}
