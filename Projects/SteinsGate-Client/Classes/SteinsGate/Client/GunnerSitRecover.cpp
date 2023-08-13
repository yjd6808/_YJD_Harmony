/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 7:02:29 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GunnerSitRecover.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Animation.h>

GunnerSitRecover::GunnerSitRecover(HostPlayer* player, ActionInfo* actionInfo)
	: GunnerAction(player, actionInfo)
{
}

void GunnerSitRecover::onActionBegin() {
	m_fElaspedSitTime = 0.0f;
	m_fRecoverSitTime = m_pBaseInfo->DownRecoverTime / 2.0f;
	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_SIT);
}

void GunnerSitRecover::onUpdate(float dt) {
	m_fElaspedSitTime += dt;

	if (m_fElaspedSitTime >= m_fRecoverSitTime) {
		stop();
	}
}

