/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "AttackActivity.h"

//////////////////////////////////////////////////////////////////////////////////////////
AttackActivity::AttackActivity(Actor* _pActor)
: AIActivity(_pActor, AIActivityType::Attack)
, pHitRecorder_(nullptr)
{
}
