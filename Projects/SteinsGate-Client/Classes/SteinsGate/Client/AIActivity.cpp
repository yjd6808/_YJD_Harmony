/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:58:10 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "AIActivity.h"

AIActivity::AIActivity(Actor* actor, AIActivityType_t type)
	: m_pActor(actor)
	, m_eType(type)
	, m_eState(eInitialized)
	, m_fElasedTime(0.0f)
	, m_fLimitTime(1.0f)
{}

void AIActivity::run() {
	DebugAssertMsg(m_eState != eRunning, "해당 AI 액터의 액티비티가 실행중입니다.");
	m_eState = eRunning;
	m_fElasedTime = 0.0f;
	onActivityBegin();
}

void AIActivity::stop() {
	if (!isRunning())
		return;

	m_eState = eFinished;
	onActivityEnd();
}

bool AIActivity::isRunning() {
	return m_eState == eRunning;
}

void AIActivity::updateLimitTime(float dt) {
	if (!isRunning())
		return;

	m_fElasedTime += dt;

	if (m_fElasedTime >= m_fLimitTime) {
		stop();
	}
}
