/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 7:00:11 PM
 * =====================
 *
 */


#include "SGPhysicsActor.h"

#include <SteinsGate/Research/SGImagePackManager.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGMapLayer.h>

USING_NS_CC;
USING_NS_JC;

SGPhysicsActor::SGPhysicsActor(ActorType_t type, int code, SGMapLayer* mapLayer)
	: SGActor(type, code, mapLayer)
	, m_bUseElasticity(false)
	, m_fWeight(0.0f)
	, m_fUpTime(0.0f)
	, m_fDownTime(0.0f)
	, m_fVelocityX(0.0f)
	, m_fVelocityY(0.0f) {}


SGPhysicsActor::~SGPhysicsActor() {}

void SGPhysicsActor::disableElasticity() {
	m_bUseElasticity = false;
}

void SGPhysicsActor::enableElasticity() {
	m_bUseElasticity = true;
}

void SGPhysicsActor::update(float dt) {
	SGActor::update(dt);

	updatePhysics(dt);
}

void SGPhysicsActor::updatePhysics(float dt) {

	if (getType() == ActorType::Monster)
		return;

	updateGravity(dt);
	updateFriction(dt);

	// TODO: 땅바닥이 어디인지
}

void SGPhysicsActor::updateGravity(float dt) {
	SGMapInfo* pMapInfo = m_pBelongedMap->getMapInfo();

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
		if (m_fDownTime >= pMapInfo->ElasticityDownTime && m_bUseElasticity) {

			// 반탄력 느낌으로 내려올때 튕겨준다.
			// 이건 껐다 켰다 할 수 있도록.. m_bUseElasticity를 활용.
			m_fVelocityY = Math::Abs(m_fVelocityY / pMapInfo->ElasticityDividedForce);
		}
		else {
			m_fVelocityY = 0.0f;
		}

		m_fDownTime = 0.0f;
		y = 0.0f;
	}

	m_pActorSprite->setPositionY(y);
}

void SGPhysicsActor::updateFriction(float dt) {
	SGMapInfo* pMapInfo = m_pBelongedMap->getMapInfo();
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

	if (m_pBelongedMap->isCollideWithObstacles(groundRect))
		return;

	setPositionRealX(groundRect.origin.x);
}



void SGPhysicsActor::addForceX(float force) {
	m_fVelocityX += force;
}

void SGPhysicsActor::addForceY(float force) {
	float ff = m_fVelocityY;
	m_fVelocityY += force;
	Log("기존 힘: %.1f, 더한 힘: %.1f, 결과 : %.1f\n", ff, force, m_fVelocityY);
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

bool SGPhysicsActor::hasVelocity() {

	if (Math::Abs(m_fVelocityX) >= SG_FLT_EPSILON)
		return true;

	if (Math::Abs(m_fVelocityY) >= SG_FLT_EPSILON)
		return true;

	return false;
}
