/*
 * 작성자: 윤정도
 * 생성일: 1/28/2023 8:43:58 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerAction.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/HostPlayer.h>

GunnerAction::GunnerAction(HostPlayer* player, ActionInfo* actionInfo)
	: SGAction(player, actionInfo)
{}

GunnerAction::~GunnerAction() {
	
}

void GunnerAction::play() {
	m_pBaseInfo = dynamic_cast<GunnerInfo*>(Core::DataManager->getCharInfo(CharType::Gunner));
	DebugAssertMsg(m_pBaseInfo, "오잉! 거너 인포가 아닌데요?");

	SGAction::play();
}

