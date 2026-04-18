/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Game/AI/Activity/FallDown/FallDownActivity.h"

#include "sgcl/Game/Actor/Monster.h"
#include "sgcl/Define/Define_Animation.h"
#include "sgcl/Game/Component/PhysicsComponent.h"

//////////////////////////////////////////////////////////////////////////////////////////
FallDownActivity::FallDownActivity(Actor* _pActor)
: AIActivity(_pActor, AIActivityType::FallDown)
{
}
