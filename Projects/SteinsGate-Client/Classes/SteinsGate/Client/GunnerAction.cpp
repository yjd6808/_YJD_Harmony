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

//////////////////////////////////////////////////////////////////////////////////////////
GunnerAction::GunnerAction(HostPlayer* _pHostPlayer, ActionInfo* _pActionInfo)
: SGAction(_pHostPlayer, _pActionInfo)
{
}

GunnerAction::~GunnerAction()
{
}

void GunnerAction::Play()
{
	pBaseInfo_ = dynamic_cast<GunnerInfo*>(Core::DataManager->GetCharInfo(CharType::Gunner));
	DebugAssertMsg(pBaseInfo_, "오잉! 거너 인포가 아닌데요?");

	SGAction::Play();
}
