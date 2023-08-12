/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:17:26 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SitActivity.h"

SitActivity::SitActivity(Actor* actor)
	: AIActivity(actor, AIActivityType::Idle) {}

void SitActivity::onUpdate(float dt) {
	updateLimitTime(dt);
}

