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
	m_pBaseInfo = (SGGunnerBaseInfo*)SGDataManager::get()->getCharBaseInfo(CharType::Gunner);
	m_eWeaponType = GunnerWeaponType::Auto;		// TODO: 무기 정보 읽기

	SGAction::play();
}

