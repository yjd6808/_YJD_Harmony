/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 8:01:53 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "HitActivity.h"

HitActivity::HitActivity(Actor* actor)
	: AIActivity(actor, AIActivityType::Hit)
{}

