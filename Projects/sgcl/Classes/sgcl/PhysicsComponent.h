/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:05:53 AM
 * =====================
 *
 */

#pragma once

#include <sgcl/ActorComponent.h>

struct HitInfo;

class PhysicsComponent : public ActorComponent, public IUpdatable
{
public:
	PhysicsComponent(Actor* _pActor);

	void Initialize() override;
	void OnUpdate(float _dt) override;

	void Hit(const HitInfo& _hitInfo);

	bool IsPaused();
	bool IsBounced();

	void DisableElasticity();
	void EnableElasticity();

	void UpdatePauseTime(float _dt);
	void UpdatePhysics(float _dt);
	void UpdateGravity(float _dt);
	void UpdateFriction(float _dt);

	// 디버깅용 코드 =====================
	void UpdateDebug(float _dt);
	void UpdateDebugSub1(float _dt);
	void UpdateDebugSub2(float _dt);
	// =================================

	float AddForceX(float _force);
	float AddForceY(float _force);

	float RemoveForceX();
	float RemoveForceY();

	float GetUpTime() { return upTime_; }
	float GetDownTime() { return downTime_; }
	float GetForceX() { return velocity_.x; }
	float GetForceY() { return velocity_.y; }
	float GetWeight() { return weight_; }

	bool HasForceX();
	bool HasForceY();
	bool HasForce();

	void PausePhysics();
	void PausePhysics(float _time);
	void Resume();

	void StiffenBody(float _time); // 경직시키다 영단어

	Direction_t GetForceXDirection();
	Direction_t GetForceYDirection();

	SG_COMPONENT_TYPE_GETTER(Type::ePhysics)

private:
	bool useElasticity_;
	bool bounced_;

	cc::vec2 velocity_;

	float weight_;
	float upTime_;
	float downTime_;
	float elapsedPausedTime_;
	float pauseTime_;

	// 디버깅 용
	float atkBoxInstantElapsedTime_;
	cc::DrawNode* atkThicknessBox_;
	cc::DrawNode* atkHitBox_;
};
