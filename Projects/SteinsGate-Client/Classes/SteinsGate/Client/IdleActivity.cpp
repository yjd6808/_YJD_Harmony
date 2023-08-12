/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "IdleActivity.h"

#include <SteinsGate/Client/Monster.h>
#include <SteinsGate/Client/Define_Animation.h>

IdleActivity::IdleActivity(Actor* actor)
	: AIActivity(actor, AIActivityType::Idle)
{}