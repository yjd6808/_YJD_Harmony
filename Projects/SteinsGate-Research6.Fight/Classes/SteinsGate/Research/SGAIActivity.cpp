/*
 * 작성자: 윤정도
 * 생성일: 1/29/2023 4:58:10 AM
 * =====================
 *
 */


#include "SGAIActivity.h"

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGAIActor.h>
#include <SteinsGate/Research/SGActorPartAnimation.h>

SGAIActivity::SGAIActivity(SGAIActor* actor, AIActivity_t type)
	: m_pActor(actor)
	, m_eState(eInitialized)
	, m_eType(type)
{}

void SGAIActivity::run() {
	DebugAssertMessage(m_eState != eRunning, "해당 AI 액터의 액티비티가 실행중입니다.");
	m_eState = eRunning;
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

void SGAIActivity::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	SGFrameInfo* pFrameInfo = animation->getFrameInfo(animation->getFrameIndexInAnimation());

	return;

	if (pFrameInfo->FrameEventId != InvalidValue_v) {
		DebugAssertMessage(pFrameInfo->FrameEvent >= 0 && pFrameInfo->FrameEvent < FrameEventType::Max, "AI 액터의 프레임 이벤트 타입이 이상합니다.");
		DebugAssertMessage(pFrameInfo->FrameEventId != InvalidValue_v, "AI 액터의 프레임 이벤트 ID 값이 이상합니다.");
		m_pActor->runFrameEvent((FrameEventType_t)pFrameInfo->FrameEvent, pFrameInfo->FrameEventId);
	}
}

void SGAIActivity::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAIActivity::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAIActivity::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
