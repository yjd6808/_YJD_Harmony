/*
 * 작성자: 윤정도
 * 생성일: 1/27/2023 8:03:41 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerHit : public GunnerAction
{
public:
	GunnerHit(HostPlayer* _pPlayer, ActionInfo* _pActionInfo);

	void onActionBegin() override;
	void onActionEnd() override;
	void selectHitAnimation();
	void checkPosition();
	void updateGroundHitState(float _deltaTime);
	void updateDownState(float _deltaTime);
	void updateAirHitState(float _deltaTime);
	void onUpdate(float _deltaTime) override;

private:
	bool hitSmall_;
	bool onTheGround_; // 초기 Hit 판정시 공중이었는지 아니면 바닥이었는지
	bool downTimeCheckBegin_;
	float elapsedDownTime_;
	float downRecoverTime_;
};
