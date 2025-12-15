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

//////////////////////////////////////////////////////////////////////////////////////////
GunnerSitRecover::GunnerSitRecover(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerSitRecover::onActionBegin()
{
	elapsedSitTime_ = 0.0f;
	recoverSitTime_ = m_pBaseInfo->downRecoverTime_ / 2.0f;
	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_SIT);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerSitRecover::onUpdate(float _dt)
{
	elapsedSitTime_ += _dt;

	if (elapsedSitTime_ >= recoverSitTime_)
	{
		stop();
	}
}
