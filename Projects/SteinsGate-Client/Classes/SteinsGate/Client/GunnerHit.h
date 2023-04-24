/*
 * 작성자: 윤정도
 * 생성일: 1/27/2023 8:03:41 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerHit : public GunnerAction {
public:
	GunnerHit(HostPlayer* player, ActionInfo* actionInfo);

	void onActionBegin() override;
	void onActionEnd() override;
	void selectHitAnimation();
    void checkPosition();
    void updateGroundHitState(float dt);
    void updateDownState(float dt);
    void updateAirHitState(float dt);
    void onUpdate(float dt) override;
private:
	bool m_bHitSmall;
	bool m_bOnTheGround;		// 초기 Hit 판정시 공중이었는지 아니면 바닥이었는지
	bool m_bDownTimeCheckBegin;
	float m_fElapsedDownTime;
	float m_fDownRecoverTime;
};
