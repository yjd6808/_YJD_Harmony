/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:36:46 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "MonsterActivity.h"

#include <SteinsGate/Client/Monster.h>

MonsterActivity::MonsterActivity(Monster* monster, AIActivityType_t type)
	: AIActivity(type)
	, m_pMonster(monster) {}

void MonsterActivity::run() {
	AIActivity::run();
}

void MonsterActivity::stop() {
	AIActivity::stop();
}

AIActor* MonsterActivity::getAIActor() {
	return m_pMonster;
}
