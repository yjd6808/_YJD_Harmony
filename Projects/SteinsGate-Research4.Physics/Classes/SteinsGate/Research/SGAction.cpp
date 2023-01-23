/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:07:50 AM
 * =====================
 *
 */




#include "SGAction.h"

#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGComboAction.h>
#include <JCore/Utils/Console.h>

#include "SGMapLayer.h"

SGAction::SGAction(SGPlayer* player, SGActionInfo* actionInfo)
	: m_pPlayer(player)
	, m_pActionInfo(actionInfo)
	, m_bMoveable(m_pActionInfo->Moveable)
	, m_bCancelable(m_pActionInfo->ForceCancelable)
	, m_bChangeDirection(m_pActionInfo->ChangeDirection)
	, m_fMoveSpeedFPSX(m_pActionInfo->SpeedX)
	, m_fMoveSpeedFPSY(m_pActionInfo->SpeedY)
{}

void SGAction::play() {
	// 플레이어가 사용가능한지 체크

	onActionBegin();
}

void SGAction::stop() {

	
	m_pPlayer->getActionManager()->setRunningAction(nullptr);	// 완료된 액션은 먼저 바로 해제 해준다.
	m_pPlayer->getController()->setCommandable(true);			// 다시 콤보 입력이 가능하도록 변경해준다.

	if (!isForceCancelable())
		m_pPlayer->getController()->reflectPressedMoveKeys();
	

	onActionEnd();
}

void SGAction::onKeyPressed(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

}

void SGAction::onKeyReleased(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

}


void SGAction::onKeyPressedBefore(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

}

void SGAction::onKeyReleasedBefore(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

}

void SGAction::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAction::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAction::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAction::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {}


void SGAction::onActionBegin() {
#ifdef DebugMode
	if (isComboAction()) {
		SGComboAction* pComboAction = (SGComboAction*)this;
		// Log("%s", pComboAction->getComboKeys().String().Source());
	}
#endif
}


void SGAction::onActionEnd() {
}

void SGAction::runFrameEvent(FrameEventType_t frameEventType, int frameEventId) {
	SGMapLayer* pMapLayer = m_pPlayer->getMapLayer();
	DebugAssertMessage(pMapLayer, "플레이가 속한 맵 레이어가 없습니다.");
	pMapLayer->runFrameEvent(m_pPlayer->getCharacter(), frameEventType, frameEventId);
}
