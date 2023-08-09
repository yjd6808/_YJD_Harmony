/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:05:53 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/ActorComponent.h>

class PhysicsComponent : public ActorComponent, public IUpdatable
{
public:
	PhysicsComponent(Actor* actor);

	void initVariables();

	void onUpdate(float dt) override;

	void hit(const HitInfo& info);

	bool isPaused();
	bool isBounced();

	void disableElasticity();
	void enableElasticity();

	void updatePauseTime(float dt);
	void updatePhysics(float dt);
	void updateGravity(float dt);
	void updateFriction(float dt);

	// 디버깅용 코드 =====================
	void updateDebug(float dt);
	void updateDebugSub1(float dt);
	void updateDebugSub2(float dt);
	// =================================

	float addForceX(float force);
	float addForceY(float force);

	float removeForceX();
	float removeForceY();

	float getUpTime() { return m_fUpTime; }
	float getDownTime() { return m_fDownTime; }
	float getForceX() { return m_Velocity.x; }
	float getForceY() { return m_Velocity.y; }
	float getWeight() { return m_fWeight; }

	bool hasForceX();
	bool hasForceY();
	bool hasForce();

	void pausePhysics();
	void pausePhysics(float time);
	void resume();

	void stiffenBody(float time);		// 경직시키다 영단어

	Direction_t getForceXDirection();
	Direction_t getForceYDirection();

	SG_COMPONENT_TYPE_GETTER(Type::ePhysics)
private:
	bool m_bUseElasticity;
	bool m_bBounced;

	SGVec2 m_Velocity;

	float m_fWeight;
	float m_fUpTime;
	float m_fDownTime;
	float m_fElapsedPausedTime;
	float m_fPuaseTime;

	// 디버깅 용
	float m_fAtkBoxInstantElapsedTime;
	SGDrawNode* m_pAtkThicknessBox;
	SGDrawNode* m_pAtkHitBox;
};




