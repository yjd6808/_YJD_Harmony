/*
 * 작성자: 윤정도
 * 생성일: 2/1/2023 10:36:46 AM
 * =====================
 *
 */


#include "SGMonsterActivity.h"

#include <SteinsGate/Research/SGMonster.h>

SGMonsterActivity::SGMonsterActivity(SGMonster* monster, AIActivity_t type)
	: SGAIActivity(type)
	, m_pMonster(monster) {}

void SGMonsterActivity::run() {
	m_pMonster->getHitRecorder()->clear();
	m_pMonster->getHitRecorder()->setSingleHitCallback(nullptr);
	m_pMonster->getHitRecorder()->setMultiHitCallback(nullptr);

	SGAIActivity::run();
}

void SGMonsterActivity::stop() {
	SGAIActivity::stop();
}

SGAIActor* SGMonsterActivity::getAIActor() {
	return m_pMonster;
}
