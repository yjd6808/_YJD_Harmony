/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:01 AM
 * =====================
 *
 */


#include "SGPlayer.h"

#include <SteinsGate/Research/SGCharacterBaseInfo.h>

SGPlayer::SGPlayer()
	: m_pMapLayer(nullptr)
	, m_pCharacter(nullptr)
	, m_pController(nullptr)
	, m_pActionManager(nullptr) {
	
}

SGPlayer::~SGPlayer() {
	CC_SAFE_RELEASE_NULL(m_pCharacter);

	// 삭제
	CC_SAFE_DELETE(m_pController);
	CC_SAFE_DELETE(m_pActionManager);
}

void SGPlayer::initActionManager() {
	DebugAssertMessage(m_pCharacter, "이 함수를 호출전에 무조건 캐릭터 세팅을 먼저 해주세요.");
	m_pActionManager = new SGActionManager(this);
	m_pActionManager->init(m_pCharacter->getBaseInfo()->Type);
}

void SGPlayer::initController() {
	DebugAssertMessage(m_pCharacter && m_pActionManager, "이 함수를 호출전에 무조건 캐릭터 세팅과 액션 매니저 세팅을 먼저 해주세요.");
	m_pController = new SGPlayerController(this, m_pCharacter, m_pActionManager);
}

void SGPlayer::update(float dt) {

	if (m_pController)
		m_pController->update(dt);

	if (m_pActionManager)
		m_pActionManager->update(dt);


}

void SGPlayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (m_pController)
		m_pController->onKeyPressed(keyCode, event);
}

void SGPlayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (m_pController)
		m_pController->onKeyReleased(keyCode, event);
}

void SGPlayer::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onFrameBegin(animation, texture);
}

void SGPlayer::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onFrameEnd(animation, texture);
}

void SGPlayer::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onAnimationBegin(animation, texture);
}

void SGPlayer::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onAnimationEnd(animation, texture);
}

void SGPlayer::runAction(int actionCode) {
	m_pActionManager->runAction(actionCode);
}

void SGPlayer::runActionForce(int actionCode) {
	m_pActionManager->stopActionForce();
	m_pActionManager->runAction(actionCode);
}

void SGPlayer::runBaseAction(BaseAction_t baseAction) {
	m_pActionManager->runBaseAction(baseAction);
}

void SGPlayer::runAnimation(int animationCode) {
	DebugAssertMessage(m_pCharacter, "캐릭터가 세팅되지 않았습니다.");
	m_pCharacter->runAnimation(animationCode);
}

int SGPlayer::getRunningActionCode() {
	SGAction* pAction = m_pActionManager->getRunningAction();
	if (pAction == nullptr) return InvalidValue_v;
	return pAction->getActionCode();
}

SGActorSprite* SGPlayer::getActorSprite() {
	return m_pCharacter->getActorSprite();
}


void SGPlayer::setCharacter(SGCharacter* character) {
	DebugAssertMessage(m_pCharacter == nullptr, "이미 캐릭터가 세팅되어 있습니다.");
	m_pCharacter = character;
	m_pCharacter->retain();
	m_pCharacter->setOwner(true);
}

void SGPlayer::setMapLayer(SGMapLayer* mapLayer) {
	m_pMapLayer = mapLayer;
}

SGCharacter* SGPlayer::getCharacter() {
	DebugAssertMessage(m_pCharacter, "캐릭터가 세팅되지 않았습니다.");
	return m_pCharacter;
}

SGActionManager* SGPlayer::getActionManager() {
	DebugAssertMessage(m_pActionManager, "액션 매니저가 세팅되지 않았습니다.");
	return m_pActionManager;
}

SGPlayerController* SGPlayer::getController() {
	DebugAssertMessage(m_pController, "플레이어 컨트롤러가 세팅되지 않았습니다.");
	return m_pController;
}

SGMapLayer* SGPlayer::getMapLayer() {
	DebugAssertMessage(m_pMapLayer, "맵 레이어가 초기화되지 않았습니다.");
	return m_pMapLayer;
}

