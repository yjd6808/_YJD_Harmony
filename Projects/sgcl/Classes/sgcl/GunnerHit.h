/*
 * 작성자: 윤정도
 * 생성일: 1/27/2023 8:03:41 AM
 * =====================
 *
 */

#pragma once

#include <sgcl/GunnerAction.h>

class GunnerHit : public GunnerAction
{
public:
	GunnerHit(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	void OnActionBegin() override;
	void OnActionEnd() override;
	void SelectHitAnimation();
	void CheckPosition();
	void UpdateGroundHitState(float _deltaTime);
	void UpdateDownState(float _deltaTime);
	void UpdateAirHitState(float _deltaTime);
	void OnUpdate(float _deltaTime) override;

private:
	bool hitSmall_;
	bool onTheGround_; // 초기 Hit 판정시 공중이었는지 아니면 바닥이었는지
	bool downTimeCheckBegin_;
	float elapsedDownTime_;
	float downRecoverTime_;
};
