/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGHostPlayer.h"

#include <SteinsGate/Client/SGCharBaseInfo.h>
#include <SteinsGate/Client/SGMapLayer.h>

SGHostPlayer::SGHostPlayer()
	: SGPlayer()
	, m_pController(nullptr)
	, m_pActionManager(nullptr) {
	
}

SGHostPlayer::~SGHostPlayer() {
	// 삭제
	CC_SAFE_DELETE(m_pController);
	CC_SAFE_DELETE(m_pActionManager);
}

void SGHostPlayer::initActionManager() {
	DebugAssertMsg(m_pCharacter, "이 함수를 호출전에 무조건 캐릭터 세팅을 먼저 해주세요.");

	CC_SAFE_DELETE(m_pActionManager);

	m_pActionManager = dbg_new SGActionManager(this);
	m_pActionManager->init(m_pCharacter->getBaseInfo()->Type);
}

void SGHostPlayer::initController() {
	CC_SAFE_DELETE(m_pController);
	DebugAssertMsg(m_pCharacter && m_pActionManager, "이 함수를 호출전에 무조건 캐릭터 세팅과 액션 매니저 세팅을 먼저 해주세요.");
	m_pController = dbg_new SGPlayerController(this, m_pCharacter, m_pActionManager);
}

void SGHostPlayer::update(float dt) {

	if (m_pController)
		m_pController->update(dt);

	if (m_pActionManager)
		m_pActionManager->update(dt);


}

void SGHostPlayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (m_pController)
		m_pController->onKeyPressed(keyCode, event);
}

void SGHostPlayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (m_pController)
		m_pController->onKeyReleased(keyCode, event);
}

void SGHostPlayer::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onFrameBegin(animation, texture);
}

void SGHostPlayer::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onFrameEnd(animation, texture);
}

void SGHostPlayer::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onAnimationBegin(animation, texture);
}

void SGHostPlayer::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onAnimationEnd(animation, texture);
}

void SGHostPlayer::runAction(int actionCode) {
	m_pActionManager->runAction(actionCode);
}

void SGHostPlayer::runActionForce(int actionCode) {
	m_pActionManager->stopActionForce();
	m_pActionManager->runAction(actionCode);
}

void SGHostPlayer::runBaseAction(BaseAction_t baseAction) {
	m_pActionManager->runBaseAction(baseAction);
}

void SGHostPlayer::runAnimation(int animationCode) {
	DebugAssertMsg(m_pCharacter, "캐릭터가 세팅되지 않았습니다.");
	m_pCharacter->runAnimation(animationCode);
}

int SGHostPlayer::getRunningActionCode() {
	SGAction* pAction = m_pActionManager->getRunningAction();
	if (pAction == nullptr) return InvalidValue_v;
	return pAction->getActionCode();
}


SGActionManager* SGHostPlayer::getActionManager() {
	DebugAssertMsg(m_pActionManager, "액션 매니저가 세팅되지 않았습니다.");
	return m_pActionManager;
}

SGPlayerController* SGHostPlayer::getController() {
	DebugAssertMsg(m_pController, "플레이어 컨트롤러가 세팅되지 않았습니다.");
	return m_pController;
}


