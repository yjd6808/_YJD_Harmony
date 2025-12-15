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
	PhysicsComponent(Actor* _pActor);

	void initialize() override;
	void onUpdate(float _dt) override;

	void hit(const HitInfo& _hitInfo);

	bool isPaused();
	bool isBounced();

	void disableElasticity();
	void enableElasticity();

	void updatePauseTime(float _dt);
	void updatePhysics(float _dt);
	void updateGravity(float _dt);
	void updateFriction(float _dt);

	// 디버깅용 코드 =====================
	void updateDebug(float _dt);
	void updateDebugSub1(float _dt);
	void updateDebugSub2(float _dt);
	// =================================

	float addForceX(float _force);
	float addForceY(float _force);

	float removeForceX();
	float removeForceY();

	float getUpTime() { return upTime_; }
	float getDownTime() { return downTime_; }
	float getForceX() { return velocity_.x; }
	float getForceY() { return velocity_.y; }
	float getWeight() { return weight_; }

	bool hasForceX();
	bool hasForceY();
	bool hasForce();

	void pausePhysics();
	void pausePhysics(float _time);
	void resume();

	void stiffenBody(float _time); // 경직시키다 영단어

	Direction_t getForceXDirection();
	Direction_t getForceYDirection();

	SG_COMPONENT_TYPE_GETTER(Type::ePhysics)

private:
	bool useElasticity_;
	bool bounced_;

	SGVec2 velocity_;

	float weight_;
	float upTime_;
	float downTime_;
	float elapsedPausedTime_;
	float pauseTime_;

	// 디버깅 용
	float atkBoxInstantElapsedTime_;
	SGDrawNode* atkThicknessBox_;
	SGDrawNode* atkHitBox_;
};
