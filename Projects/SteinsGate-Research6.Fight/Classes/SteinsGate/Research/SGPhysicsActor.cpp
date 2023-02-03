/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 7:00:11 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGPhysicsActor.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGMapLayer.h>
#include <SteinsGate/Research/SGGlobal.h>

#include <SteinsGate/Common/Engine/RectPoly.h>

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

	// TODO: 땅바닥이 어디인지
}

void SGPhysicsActor::updateGravity(float dt) {
	SGMapInfo* pMapInfo = m_pMapLayer->getMapInfo();

	// 이번틱에 이동할 y축 위치
	float y = m_pActorSprite->getPositionY() + m_fVelocityY * FPS1_v;
	m_fVelocityY -= pMapInfo->Gravity * FPS1_v;

	// 공중에 뜬 상태에서 올라가는 시간을 계산한다.
	if (y > SG_FLT_EPSILON && m_fVelocityY > 0.0f) {
		m_fUpTime += dt;
		m_fDownTime = 0.0f;
	}

	// 공중에 뜬 상태에서 하락중(jump_velocity가 음수로 전환된 시점)일 때 하락시간을 계산한다.
	if (y > SG_FLT_EPSILON && m_fVelocityY < 0.0f) {
		m_fUpTime = 0.0f;
		m_fDownTime += dt;
	}

	if (y <= SG_FLT_EPSILON) {
		if (m_bUseElasticity && !m_bBounced && m_fDownTime > 0.0f) {

			// 반탄력 느낌으로 내려올때 튕겨준다.
			// 이건 껐다 켰다 할 수 있도록.. m_bUseElasticity를 활용.
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

	// y좌표가 지면에 닫은 경우 마찰력 적용
	if (m_pActorSprite->getPositionY() <= SG_FLT_EPSILON) {

		// 오른쪽으로 밀리는 경우 왼쪽 방향으로 마찰력
		if (m_fVelocityX > 0.0f) {
			m_fVelocityX -= pMapInfo->Friction * FPS1_v;

			// 이때 음수로 전환된 경우 완전히 멈춘 경우임
			if (m_fVelocityX <= 0.0f) {
				m_fVelocityX = 0.0f;
			}
		}
		else if (m_fVelocityX < 0.0f) { // 왼쪽으로 밀리는 경우 오른쪽 방향으로 마찰력 
			m_fVelocityX += pMapInfo->Friction * FPS1_v;

			// 이때 양수로 전환된 경우 완전히 멈춘 경우임
			if (m_fVelocityX >= 0.0f) {
				m_fVelocityX = 0.0f;
			}
		}
	}

	// 물리는 지속 업데이트 시켜놔야함
	// 마지막 이동 판정을 못하도록 한다.
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
	hit(hitInfo.HitDirection, hitInfo.HitRect, hitInfo.AttackDataInfo);
}

void SGPhysicsActor::hit(
	const SpriteDirection_t hitDirection, 
	const SGRect& hitRect,
	SGAttackDataInfo* attackDataInfo) {
	float fForceX = hitDirection == SpriteDirection::Right ? -attackDataInfo->AttackXForce : attackDataInfo->AttackXForce;
	m_bBounced = false;

	setSpriteDirection(hitDirection);
	removeForceX();
	removeForceY();

	// 넘어지는 모션을 취하게 하는 경우
	if (attackDataInfo->IsFallDownAttack) {
		addForceX(fForceX);
		addForceY(attackDataInfo->AttackYForce);
		return;
	}

	// 맞는 모션을 취하게 하는 경우
	// 땅위에 있는 경우 X축 힘만 가한다.
	if (isOnTheGround()) {
		addForceX(fForceX);
		return;
	}

	// 공중에 있는 경우 Y축 힘만 가한다.
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

	if (!SGGlobal::getInstance()->isAttackBoxDrawMode())
		return;

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
