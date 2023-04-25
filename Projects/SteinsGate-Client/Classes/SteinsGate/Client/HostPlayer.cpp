/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "HostPlayer.h"

#include <SteinsGate/Client/CharInfo.h>
#include <SteinsGate/Client/MapLayer.h>
#include <SteinsGate/Client/ActionDefine.h>
#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Client/Inven.h>


HostPlayer::HostPlayer()
	: Player()
	, m_pController(nullptr)
	, m_pActionManager(nullptr) {
	
}

HostPlayer::~HostPlayer() {
	// 삭제
	CC_SAFE_DELETE(m_pController);
	CC_SAFE_DELETE(m_pActionManager);
}

bool HostPlayer::init() {

	m_PlayerData.CharType = CharType::Gunner;

	VisualInfo info;
	CoreInven_v->getVisualInfo(info, m_PlayerData.CharType);


	initInfo(m_PlayerData.CharType, info);
	initActionManager();
	initController();
	m_pActionManager->runBaseAction(BaseAction::Idle);

	return true;
}

void HostPlayer::initActionManager() {
	CC_SAFE_DELETE(m_pActionManager);

	m_pActionManager = dbg_new ActionMgr(this);
	m_pActionManager->init(getBaseInfo()->Code);
}

void HostPlayer::initController() {
	CC_SAFE_DELETE(m_pController);
	DebugAssertMsg(m_pActionManager, "이 함수를 호출전에 액션 매니저 세팅을 먼저 해주세요.");
	m_pController = dbg_new PlayerController(this, m_pActionManager);
}

void HostPlayer::hit(const SGHitInfo& hitInfo) {
	Player::hit(hitInfo);

	if (hitInfo.AttackDataInfo->IsFallDownAttack) {
		playBaseActionForce(BaseAction::FallDown);
		return;
	}

	playBaseActionForce(BaseAction::Hit);
}

void HostPlayer::update(float dt) {

	Player::update(dt);

	if (m_pController)
		m_pController->update(dt);

	if (m_pActionManager)
		m_pActionManager->update(dt);

}

void HostPlayer::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	if (m_pController)
		m_pController->onKeyPressed(keyCode, event);
}

void HostPlayer::onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	if (m_pController)
		m_pController->onKeyReleased(keyCode, event);
}

void HostPlayer::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onFrameBegin(animation, texture);
}

void HostPlayer::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onFrameEnd(animation, texture);
}

void HostPlayer::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onAnimationBegin(animation, texture);
}

void HostPlayer::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pActionManager)
		m_pActionManager->onAnimationEnd(animation, texture);
}

void HostPlayer::playAction(int actionCode) {
	m_pActionManager->runAction(actionCode);
}

void HostPlayer::playActionForce(int actionCode) {
	m_pActionManager->stopActionForce();
	m_pActionManager->runAction(actionCode);
}

void HostPlayer::playBaseActionForce(BaseAction_t baseActionType) {
	m_pActionManager->stopActionForce();
	m_pActionManager->runBaseAction(baseActionType);
}

void HostPlayer::playBaseAction(BaseAction_t baseAction) {
	m_pActionManager->runBaseAction(baseAction);
}

int HostPlayer::getRunningActionCode() {
	SGAction* pAction = m_pActionManager->getRunningAction();
	if (pAction == nullptr) return InvalidValue_v;
	return pAction->getActionCode();
}


ActionMgr* HostPlayer::actionManager() {
	DebugAssertMsg(m_pActionManager, "액션 매니저가 세팅되지 않았습니다.");
	return m_pActionManager;
}

PlayerController* HostPlayer::ctrl() {
	DebugAssertMsg(m_pController, "플레이어 컨트롤러가 세팅되지 않았습니다.");
	return m_pController;
}

