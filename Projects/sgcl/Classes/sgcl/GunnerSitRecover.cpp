/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 7:02:29 PM
 * =====================
 *
 */

#include "Core.h"
#include "GunnerSitRecover.h"

#include <sgcl/HostPlayer.h>
#include <sgcl/Define_Animation.h>

//////////////////////////////////////////////////////////////////////////////////////////
GunnerSitRecover::GunnerSitRecover(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerSitRecover::OnActionBegin()
{
	elapsedSitTime_ = 0.0f;
	recoverSitTime_ = pBaseInfo_->downRecoverTime_ / 2.0f;
	pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_SIT);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerSitRecover::OnUpdate(float _dt)
{
	elapsedSitTime_ += _dt;

	if (elapsedSitTime_ >= recoverSitTime_)
	{
		Stop();
	}
}
