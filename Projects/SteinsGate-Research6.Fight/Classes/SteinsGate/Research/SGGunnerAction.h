/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 8:14:43 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGAction.h>
#include <SteinsGate/Research/SGCharacterBaseInfo.h>

class SGGunnerAction : public SGAction {
public:
	SGGunnerAction(SGPlayer* player, SGActionInfo* actionInfo);
	~SGGunnerAction() override;

	void play() override;
protected:
	
	SGGunnerBaseInfo* m_pBaseInfo;
	GunnerWeaponType_t m_eWeaponType;
};


