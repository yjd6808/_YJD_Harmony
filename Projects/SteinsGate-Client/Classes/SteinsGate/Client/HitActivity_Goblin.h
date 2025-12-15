/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:39 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/HitActivity.h>

class HitActivity_Goblin : public HitActivity
{
public:
	HitActivity_Goblin(Actor* _pActor);

	void selectHitAnimation();
	void checkPosition();
	void OnActivityBegin() override;
	void updateGroundHitState(float _dt);
	void updateDownState(float _dt);
	void updateAirHitState(float _dt);
	void OnUpdate(float _dt) override;

private:
	bool hitSmall_;
	bool onTheGround_; // 초기 Hit 판정시 공중이었는지 아니면 바닥이었는지 (false시 공중)
	bool downTimeCheckBegin_;
	float elapsedDownTime_;
	float downRecoverTime_;
};
