/*
 * 작성자: 윤정도
 * 생성일: 1/26/2023 6:58:56 PM
 * =====================
 * 짝퉁 물리가 적용될 액터
 *  - 캐릭터
 *	- 몬스터
 */


#pragma once

#include <SteinsGate/Research/SGActor.h>

class SGPhysicsActor : public SGActor
{
public:
	SGPhysicsActor(ActorType_t type, int code, SGMapLayer* mapLayer);
	~SGPhysicsActor();

	bool isPhysicsActor() override { return true; }
	void setWeight(float weight) { m_fWeight = weight; }
	void disableElasticity();
	void enableElasticity();

	void update(float dt) override;
	void updatePhysics(float dt);
	void updateGravity(float dt);
	void updateFriction(float dt);

	float addForceX(float force);
	float addForceY(float force);

	float removeForceX();
	float removeForceY();

	float getUpTime() { return m_fUpTime; }
	float getDownTime() { return m_fDownTime; }
	float getForceX() { return m_fVelocityX; }
	float getForceY() { return m_fVelocityY; }
	float getWeight() { return m_fWeight; }

	bool hasForceX();
	bool hasForceY();

	Direction_t getForceXDirection();
	Direction_t getForceYDirection();

	bool hasVelocity();
protected:
	bool m_bUseElasticity;

	float m_fWeight;
	float m_fUpTime;
	float m_fDownTime;
	float m_fVelocityX;
	float m_fVelocityY;
};


