/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:07:50 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGAction.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/SGHostPlayer.h>
#include <JCore/Utils/Console.h>

SGAction::SGAction(SGHostPlayer* player, SGActionInfo* actionInfo)
	: m_pPlayer(player)
	, m_pActionInfo(actionInfo)
	, m_bMoveablePositiveX(m_pActionInfo->Moveable)
	, m_bMoveablePositiveY(m_pActionInfo->Moveable)
	, m_bMoveableNegativeX(m_pActionInfo->Moveable)
	, m_bMoveableNegativeY(m_pActionInfo->Moveable)
	, m_bCancelable(m_pActionInfo->ForceCancelable)
	, m_fMoveSpeedFPSX(m_pActionInfo->SpeedX)
	, m_fMoveSpeedFPSY(m_pActionInfo->SpeedY)
	, m_pHitRecorder(nullptr)
{}

SGAction::~SGAction() {
}

void SGAction::play() {
	// 플레이어가 사용가능한지 체크
	m_pHitRecorder = m_pPlayer->getHitRecorder();
	m_pHitRecorder->clear();
	m_eWeaponType = CoreInven_v->getWeaponType();

	onActionBegin();
}

void SGAction::stop() {

	m_pPlayer->actionManager()->stopActionForce();
	m_pPlayer->ctrl()->setCommandable(true);			// 다시 콤보 입력이 가능하도록 변경해준다.

	m_pHitRecorder->setAlreadyHitRecord(false);
	m_pHitRecorder->setSingleHitCallback(nullptr);
	m_pHitRecorder->setMultiHitCallback(nullptr);

	if (!isForceCancelable()) {
		m_pPlayer->ctrl()->reflectPressedMoveKeys();
	}
	
	onActionEnd();
}


void SGAction::onActionBegin() {}
void SGAction::onActionEnd() {}
void SGAction::onKeyPressed(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {}
void SGAction::onKeyReleased(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {}
void SGAction::onKeyPressedBefore(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {}
void SGAction::onKeyReleasedBefore(SGPlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {}
void SGAction::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAction::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAction::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {}
void SGAction::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {}

void SGAction::setMoveable(bool moveable) {
	m_bMoveablePositiveX = moveable;
	m_bMoveablePositiveY = moveable;
	m_bMoveableNegativeX = moveable;
	m_bMoveableNegativeY = moveable;
}

void SGAction::runFrameEvent(FrameEventType_t frameEventType, int frameEventId) {
	DebugAssertMsg(m_pPlayer->getMapLayer() != nullptr, "플레이가 속한 맵 레이어가 없습니다.");
	m_pPlayer->runFrameEvent(frameEventType, frameEventId);
}
