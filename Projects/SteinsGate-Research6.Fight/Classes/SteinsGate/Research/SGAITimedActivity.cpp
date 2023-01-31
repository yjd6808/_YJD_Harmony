/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:58:29 AM
 * =====================
 *
 */


#include "SGAITimedActivity.h"


SGAITimedActivity::SGAITimedActivity(SGAIActor* actor, AIActivity_t type)
	: SGAIActivity(actor, type)
	, m_fElasedTime(0.0f)
	, m_fLimitTime(1.0f)
{} 

void SGAITimedActivity::setLimit(float limit) {
	m_fLimitTime = limit;
}

void SGAITimedActivity::onUpdate(float dt) {

	if (!isRunning())
		return;

	m_fElasedTime += dt;

	if (m_fElasedTime >= m_fLimitTime) {
		stop();
	}
}

void SGAITimedActivity::onActivityBegin() {
	m_fElasedTime = 0.0f;
}

