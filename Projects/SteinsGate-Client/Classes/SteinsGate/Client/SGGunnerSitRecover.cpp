/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 7:02:29 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGGunnerSitRecover.h"

#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGAnimationDefine.h>

SGGunnerSitRecover::SGGunnerSitRecover(SGHostPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo)
{
}

void SGGunnerSitRecover::onActionBegin() {
	m_fElaspedSitTime = 0.0f;
	m_fRecoverSitTime = m_pBaseInfo->DownRecoverTime / 2.0f;
	m_pPlayer->runAnimation(GUNNER_ANIMATION_SIT);
}

void SGGunnerSitRecover::onUpdate(float dt) {
	m_fElaspedSitTime += dt;

	if (m_fElaspedSitTime >= m_fRecoverSitTime) {
		stop();
	}
}

