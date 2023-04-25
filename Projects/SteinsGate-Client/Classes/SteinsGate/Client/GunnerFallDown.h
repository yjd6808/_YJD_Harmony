﻿/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 10:33:10 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/GunnerAction.h>

class GunnerFallDown : public GunnerAction {
public:
	GunnerFallDown(HostPlayer* player, ActionInfo* actionInfo);

	void onActionBegin() override;
	void onActionEnd() override;
	void onUpdate(float dt) override;
private:
	bool m_bBounced;
	bool m_bDown;
	float m_fElapsedDownTime;
	float m_fDownRecoverTime;
};

