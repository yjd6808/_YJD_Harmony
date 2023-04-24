/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 8:14:43 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGAction.h>
#include <SteinsGate/Client/CharInfo.h>

class GunnerAction : public SGAction {
public:
	GunnerAction(HostPlayer* player, ActionInfo* actionInfo);
	~GunnerAction() override;

	void play() override;
	bool onConditionCheck() override { return true; }
protected:
	GunnerInfo* m_pBaseInfo;
};


