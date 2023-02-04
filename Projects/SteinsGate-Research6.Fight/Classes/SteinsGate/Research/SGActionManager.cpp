/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:01:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGActionManager.h"

#include <SteinsGate/Research/SGAction.h>
#include <SteinsGate/Research/SGDataManager.h>
#include <SteinsGate/Research/SGAllActions.h>
#include <SteinsGate/Research/SGActionDefine.h>
#include <SteinsGate/Research/SGFrameTexture.h>


SGActionManager::SGActionManager(SGPlayer* player)
	: m_pPlayer(player)
	, m_pRunningAction(nullptr)
	, m_pPreviousAction(nullptr)
	, m_pBaseAction{{}}  {}

SGActionManager::~SGActionManager() {
	m_ActionMap.Values().Extension().ForEach([](SGAction* action) { DeleteSafe(action); });
	Log("액션 매니저 소멸\n");
}

void SGActionManager::init(int charType) {

	switch (charType) {
	case CharType::Gunner:
		initGunnerActions();
		break;
	default:
		DebugAssertMessage(false, "해당 타입의 캐릭터는 존재하지 않습니다.");
	}

	m_ActionMap.Values()
		.Extension()
		.ForEach([this](SGAction* action) {

		if (action->getCommand().count() > 0) {
			m_ComboTree.addComboAction(action);
		}
	});
}

void SGActionManager::initGunnerActions() {
	SGDataManager* pConfig = SGDataManager::getInstance();

	m_pBaseAction[BaseAction::Idle] = new SGGunnerIdle(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_IDLE));
	m_pBaseAction[BaseAction::Walk] = new SGGunnerWalk(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_WALK));
	m_pBaseAction[BaseAction::Run] = new SGGunnerRun(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_RUN));
	m_pBaseAction[BaseAction::Attack] = new SGGunnerGunShot(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_GUN_SHOT));
	m_pBaseAction[BaseAction::Sliding] = new SGGunnerSliding(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_SLIDING));
	m_pBaseAction[BaseAction::Jump] = new SGGunnerJump(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_JUMP));
	m_pBaseAction[BaseAction::Hit] = new SGGunnerHit(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_HIT));
	m_pBaseAction[BaseAction::FallDown] = new SGGunnerFallDown(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_FALL_DOWN));
	m_pBaseAction[BaseAction::SitRecover] = new SGGunnerSitRecover(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_SIT_RECOVER));
	

	m_ActionMap.Insert(GUNNER_ACTION_IDLE, m_pBaseAction[BaseAction::Idle]);
	m_ActionMap.Insert(GUNNER_ACTION_WALK, m_pBaseAction[BaseAction::Walk]);
	m_ActionMap.Insert(GUNNER_ACTION_RUN, m_pBaseAction[BaseAction::Run]);
	m_ActionMap.Insert(GUNNER_ACTION_GUN_SHOT, m_pBaseAction[BaseAction::Attack]);
	m_ActionMap.Insert(GUNNER_ACTION_SLIDING, m_pBaseAction[BaseAction::Sliding]);
	m_ActionMap.Insert(GUNNER_ACTION_JUMP, m_pBaseAction[BaseAction::Jump]);
	m_ActionMap.Insert(GUNNER_ACTION_HIT, m_pBaseAction[BaseAction::Hit]);
	m_ActionMap.Insert(GUNNER_ACTION_FALL_DOWN, m_pBaseAction[BaseAction::FallDown]);
	m_ActionMap.Insert(GUNNER_ACTION_SIT_RECOVER, m_pBaseAction[BaseAction::SitRecover]);
	
}

void SGActionManager::update(float dt) {

	if (m_pRunningAction)
		m_pRunningAction->onUpdate(dt);
}

void SGActionManager::onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyPressed(controller, keyCode);
}

void SGActionManager::onKeyReleased(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyReleased(controller, keyCode);
}

void SGActionManager::onKeyPressedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyPressedBefore(controller, keyCode);
}

void SGActionManager::onKeyReleasedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyReleasedBefore(controller, keyCode);
}

void SGActionManager::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pRunningAction)
		m_pRunningAction->onFrameBegin(animation, texture);
}

void SGActionManager::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pRunningAction)
		m_pRunningAction->onFrameEnd(animation, texture);
}

void SGActionManager::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pRunningAction)
		m_pRunningAction->onAnimationBegin(animation, texture);
}

void SGActionManager::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) {
	if (m_pRunningAction)
		m_pRunningAction->onAnimationEnd(animation, texture);
}


bool SGActionManager::canRunAction() {

	if (m_pRunningAction) {

		if (m_pRunningAction->isForceCancelable())
			return true;

		return false;	// 액션을 수행중인데 캔슬 가능한 경우가 아닌 경우
	}

	return true;
}

bool SGActionManager::isBaseActionRunning(BaseAction_t baseActionType) {
	if (m_pBaseAction[baseActionType] == nullptr)
		return false;

	return m_pBaseAction[baseActionType] == m_pRunningAction;
}

bool SGActionManager::isActionRunning(int actionCode) {
	DebugAssertMessage(m_ActionMap.Exist(actionCode), "해당 코드값의 액션이 존재하지 않습니다.");

	if (m_ActionMap[actionCode] != m_pRunningAction)
		return false;

	return true;
}

bool SGActionManager::isRunnningActionIdleOrWalkAction() {
	if (m_pRunningAction == nullptr)
		return true;

	if (m_pRunningAction == m_pBaseAction[BaseAction::Idle] ||
		m_pRunningAction == m_pBaseAction[BaseAction::Walk])
		return true;

	return false;
}


SGAction* SGActionManager::getAction(int actionCode) {
	DebugAssertMessage(m_ActionMap.Exist(actionCode), "해당 코드값의 액션이 존재하지 않습니다.");
	return m_ActionMap.Get(actionCode);
}

SGAction* SGActionManager::getBaseAction(BaseAction_t baseActionType) {
	DebugAssertMessage(m_pBaseAction[baseActionType], "해당 기초액션이 아직 구현되지 않았습니다.");
	return m_pBaseAction[baseActionType];
}

int Count = 0;

void SGActionManager::runAction(int actionCode) {
	if (!canRunAction())
		return;

	if (m_pRunningAction) {
		m_pRunningAction->stop();
	}

	m_pRunningAction = getAction(actionCode);
	m_pRunningAction->init();
	m_pRunningAction->play();
	// Log("[액션 실행 %05d] %s\n", ++Count, m_pRunningAction->getActionName().Source());
}

void SGActionManager::runAction(SGAction* action) {
	if (!canRunAction())
		return;

	if (m_pRunningAction) {
		m_pRunningAction->stop();
	}

	m_pRunningAction = action;
	m_pRunningAction->init();
	m_pRunningAction->play();
	// Log("[액션 실행 %05d] %s\n", ++Count, m_pRunningAction->getActionName().Source());

}

void SGActionManager::runBaseAction(BaseAction_t baseActionType) {
	DebugAssertMessage(m_pBaseAction[baseActionType], "해당 기초액션이 아직 구현되지 않았습니다.");
	runAction(m_pBaseAction[baseActionType]);
}

void SGActionManager::stopActionForce() {
	if (m_pRunningAction != nullptr) {
		m_pRunningAction->onActionEnd();
	}
	m_pPreviousAction = m_pRunningAction;
	m_pRunningAction = nullptr;
}

