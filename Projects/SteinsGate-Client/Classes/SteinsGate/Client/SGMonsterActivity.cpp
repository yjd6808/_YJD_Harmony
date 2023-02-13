/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:36:46 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGMonsterActivity.h"

#include <SteinsGate/Client/SGMonster.h>

SGMonsterActivity::SGMonsterActivity(SGMonster* monster, AIActivity_t type)
	: SGAIActivity(type)
	, m_pMonster(monster) {}

void SGMonsterActivity::run() {
	SGAIActivity::run();
}

void SGMonsterActivity::stop() {
	SGAIActivity::stop();
}

SGAIActor* SGMonsterActivity::getAIActor() {
	return m_pMonster;
}
