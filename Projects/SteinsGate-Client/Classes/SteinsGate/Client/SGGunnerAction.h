/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 8:14:43 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGAction.h>
#include <SteinsGate/Client/SGCharInfo.h>

class SGGunnerAction : public SGAction {
public:
	SGGunnerAction(SGHostPlayer* player, SGActionInfo* actionInfo);
	~SGGunnerAction() override;

	void play() override;
	bool onConditionCheck() override { return true; }
protected:
	SGGunnerInfo* m_pBaseInfo;
	GunnerWeaponType_t m_eWeaponType;
};


