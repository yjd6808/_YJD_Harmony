/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "FallDownActivity.h"

#include "sgcl/Monster.h"
#include "sgcl/Define_Animation.h"
#include "sgcl/PhysicsComponent.h"

//////////////////////////////////////////////////////////////////////////////////////////
FallDownActivity::FallDownActivity(Actor* _pActor)
: AIActivity(_pActor, AIActivityType::FallDown)
{
}
