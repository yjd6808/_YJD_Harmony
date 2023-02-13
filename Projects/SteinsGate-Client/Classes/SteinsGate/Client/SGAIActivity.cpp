/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:58:10 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGAIActivity.h"

#include "Tutturu.h"
#include <SteinsGate/Client/SGAIActor.h>
#include <SteinsGate/Client/SGActorPartAnimation.h>

SGAIActivity::SGAIActivity(AIActivity_t type)
	: m_eState(eInitialized)
	, m_eType(type)
	, m_fElasedTime(0.0f)
	, m_fLimitTime(1.0f)
{}

void SGAIActivity::run() {
	DebugAssertMsg(m_eState != eRunning, "해당 AI 액터의 액티비티가 실행중입니다.");
	m_eState = eRunning;
	m_fElasedTime = 0.0f;
	onActivityBegin();
}

void SGAIActivity::stop() {
	if (!isRunning())
		return;

	m_eState = eFinished;
	onActivityEnd();
}

bool SGAIActivity::isRunning() {
	return m_eState == eRunning;
}

void SGAIActivity::setLimit(float limit) {
	m_fLimitTime = limit;
}

void SGAIActivity::updateLimitTime(float dt) {
	if (!isRunning())
		return;

	m_fElasedTime += dt;

	if (m_fElasedTime >= m_fLimitTime) {
		stop();
	}
}

void SGAIActivity::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	SGFrameInfo* pFrameInfo = animation->getFrameInfo(animation->getFrameIndexInAnimation());

	return;

	if (pFrameInfo->FrameEventId != InvalidValue_v) {
		DebugAssertMsg(pFrameInfo->FrameEvent >= 0 && pFrameInfo->FrameEvent < FrameEventType::Max, "AI 액터의 프레임 이벤트 타입이 이상합니다.");
		DebugAssertMsg(pFrameInfo->FrameEventId != InvalidValue_v, "AI 액터의 프레임 이벤트 ID 값이 이상합니다.");
		getAIActor()->runFrameEvent((FrameEventType_t)pFrameInfo->FrameEvent, pFrameInfo->FrameEventId);
	}
}

void SGAIActivity::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAIActivity::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAIActivity::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
