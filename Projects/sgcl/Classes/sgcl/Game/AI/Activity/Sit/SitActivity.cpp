/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:26 AM
 * =====================
 *
 */


#include "Game/AI/Activity/Sit/SitActivity.h"

//////////////////////////////////////////////////////////////////////////////////////////
SitActivity::SitActivity(Actor* _pActor)
: AIActivity(_pActor, AIActivityType::Idle)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void SitActivity::OnUpdate(float _dt)
{
	UpdateLimitTime(_dt);
}
