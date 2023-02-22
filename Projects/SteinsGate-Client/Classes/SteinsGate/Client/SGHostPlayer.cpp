/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGHostPlayer.h"

#include <SteinsGate/Client/SGCharInfo.h>
#include <SteinsGate/Client/SGMapLayer.h>
#include <SteinsGate/Client/SGActionDefine.h>
#include <SteinsGate/Client/SGAttackDataInfo.h>
#include <SteinsGate/Client/SGInven.h>


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

bool SGHostPlayer::init() {

	m_PlayerData.CharType = CharType::Gunner;

	VisualInfo info;
	CoreInven_v->getVisualInfo(info, m_PlayerData.CharType);


	initInfo(m_PlayerData.CharType, info);
	initActionManager();
	initController();
	m_pActionManager->runBaseAction(BaseAction::Idle);

	return true;
}

void SGHostPlayer::initActionManager() {
	CC_SAFE_DELETE(m_pActionManager);

	m_pActionManager = dbg_new SGActionManager(this);
	m_pActionManager->init(getBaseInfo()->Code);
}

void SGHostPlayer::initController() {
	CC_SAFE_DELETE(m_pController);
	DebugAssertMsg(m_pActionManager, "이 함수를 호출전에 액션 매니저 세팅을 먼저 해주세요.");
	m_pController = dbg_new SGPlayerController(this, m_pActionManager);
}

void SGHostPlayer::hit(const SGHitInfo& hitInfo) {
	SGPlayer::hit(hitInfo);

	if (hitInfo.AttackDataInfo->IsFallDownAttack) {
		playBaseActionForce(BaseAction::FallDown);
		return;
	}

	playBaseActionForce(BaseAction::Hit);
}

void SGHostPlayer::update(float dt) {

	SGPlayer::update(dt);

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

void SGHostPlayer::playAction(int actionCode) {
	m_pActionManager->runAction(actionCode);
}

void SGHostPlayer::playActionForce(int actionCode) {
	m_pActionManager->stopActionForce();
	m_pActionManager->runAction(actionCode);
}

void SGHostPlayer::playBaseActionForce(BaseAction_t baseActionType) {
	m_pActionManager->stopActionForce();
	m_pActionManager->runBaseAction(baseActionType);
}

void SGHostPlayer::playBaseAction(BaseAction_t baseAction) {
	m_pActionManager->runBaseAction(baseAction);
}

int SGHostPlayer::getRunningActionCode() {
	SGAction* pAction = m_pActionManager->getRunningAction();
	if (pAction == nullptr) return InvalidValue_v;
	return pAction->getActionCode();
}


SGActionManager* SGHostPlayer::actionManager() {
	DebugAssertMsg(m_pActionManager, "액션 매니저가 세팅되지 않았습니다.");
	return m_pActionManager;
}

SGPlayerController* SGHostPlayer::ctrl() {
	DebugAssertMsg(m_pController, "플레이어 컨트롤러가 세팅되지 않았습니다.");
	return m_pController;
}


