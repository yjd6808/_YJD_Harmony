/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 8:43:58 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerAction.h"

#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGHostPlayer.h>

SGGunnerAction::SGGunnerAction(SGHostPlayer* player, SGActionInfo* actionInfo)
	: SGAction(player, actionInfo)
{}

SGGunnerAction::~SGGunnerAction() {
	
}

void SGGunnerAction::play() {
	m_pBaseInfo = dynamic_cast<SGGunnerInfo*>(SGDataManager::get()->getCharInfo(CharType::Gunner));
	DebugAssertMsg(m_pBaseInfo, "오잉! 거너 인포가 아닌데요?");
	m_eWeaponType = GunnerWeaponType::Automatic;		// TODO: 무기 정보 읽기

	SGAction::play();
}

