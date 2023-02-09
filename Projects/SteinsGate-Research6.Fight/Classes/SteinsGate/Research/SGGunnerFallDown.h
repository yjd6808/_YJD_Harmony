/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 10:33:10 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGGunnerAction.h>

class SGGunnerFallDown : public SGGunnerAction {
public:
	SGGunnerFallDown(SGHostPlayer* player, SGActionInfo* actionInfo);

	void onActionBegin() override;
	void onActionEnd() override;
	void onUpdate(float dt) override;
private:
	bool m_bBounced;
	bool m_bDown;
	float m_fElapsedDownTime;
	float m_fDownRecoverTime;
};


