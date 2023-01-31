/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 8:43:58 AM
 * =====================
 *
 */


#include "SGGunnerAction.h"

#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGPlayer.h>

SGGunnerAction::SGGunnerAction(SGPlayer* player, SGActionInfo* actionInfo)
	: SGAction(player, actionInfo)
{}

SGGunnerAction::~SGGunnerAction() {
	
}

void SGGunnerAction::onActionBegin() {
	m_pBaseInfo = (SGGunnerBaseInfo*)SGDataManager::getInstance()->getCharacterBaseInfo(CharacterType::Gunner);
	m_eWeaponType = GunnerWeaponType::Auto;		// TODO: 무기 정보 읽기
}
