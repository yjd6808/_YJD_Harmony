/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:58:10 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "AIActivity.h"

#include "Tutturu.h"
#include <SteinsGate/Client/AIActor.h>
#include <SteinsGate/Client/ActorPartAnimation.h>

AIActivity::AIActivity(AIActivityType_t type)
	: m_eState(eInitialized)
	, m_eType(type)
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

void AIActivity::setLimit(float limit) {
	m_fLimitTime = limit;
}

void AIActivity::updateLimitTime(float dt) {
	if (!isRunning())
		return;

	m_fElasedTime += dt;

	if (m_fElasedTime >= m_fLimitTime) {
		stop();
	}
}

void AIActivity::onFrameBegin(ActorPartAnimation* animation, FrameTexture* frame) {
	FrameInfo* pFrameInfo = animation->getFrameInfo(animation->getFrameIndexInAnimation());

	return;

	if (pFrameInfo->FrameEventId != InvalidValue_v) {
		DebugAssertMsg(pFrameInfo->FrameEvent >= 0 && pFrameInfo->FrameEvent < FrameEventType::Max, "AI 액터의 프레임 이벤트 타입이 이상합니다.");
		DebugAssertMsg(pFrameInfo->FrameEventId != InvalidValue_v, "AI 액터의 프레임 이벤트 ID 값이 이상합니다.");
		getAIActor()->runFrameEvent((FrameEventType_t)pFrameInfo->FrameEvent, pFrameInfo->FrameEventId);
	}
}

void AIActivity::onFrameEnd(ActorPartAnimation* animation, FrameTexture* frame) {}
void AIActivity::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* frame) {}
void AIActivity::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) {}
