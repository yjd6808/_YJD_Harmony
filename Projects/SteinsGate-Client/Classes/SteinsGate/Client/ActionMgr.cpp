/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:01:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ActionMgr.h"

#include <SteinsGate/Client/SGAction.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/AllActions.h>
#include <SteinsGate/Client/ActionDefine.h>
#include <SteinsGate/Client/FrameTexture.h>


ActionMgr::ActionMgr(HostPlayer* player)
	: m_pPlayer(player)
	, m_pRunningAction(nullptr)
	, m_pPreviousAction(nullptr)
	, m_pBaseAction{{}}  {}

ActionMgr::~ActionMgr() {
	m_ActionMap.ForEachValue([](SGAction* action) { DeleteSafe(action); });
	_LogDebug_("액션 매니저 소멸");
}

void ActionMgr::init(int charType) {

	switch (charType) {
	case CharType::Gunner:
		initGunnerActions();
		break;
	default:
		DebugAssertMsg(false, "해당 타입의 캐릭터는 존재하지 않습니다.");
	}

	m_ActionMap.Values()
		.Extension()
		.ForEach([this](SGAction* action) {

		if (action->getCommand().count() > 0) {
			m_ComboTree.addComboAction(action);
		}
	});
}

void ActionMgr::initGunnerActions() {
	DataManager* pConfig = DataManager::Get();

	m_pBaseAction[BaseAction::Idle] = dbg_new GunnerIdle(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_IDLE));
	m_pBaseAction[BaseAction::Walk] = dbg_new GunnerWalk(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_WALK));
	m_pBaseAction[BaseAction::Run] = dbg_new GunnerRun(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_RUN));
	m_pBaseAction[BaseAction::Attack] = dbg_new GunnerGunShot(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_GUN_SHOT));
	m_pBaseAction[BaseAction::Sliding] = dbg_new GunnerSliding(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_SLIDING));
	m_pBaseAction[BaseAction::Jump] = dbg_new GunnerJump(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_JUMP));
	m_pBaseAction[BaseAction::Hit] = dbg_new GunnerHit(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_HIT));
	m_pBaseAction[BaseAction::FallDown] = dbg_new GunnerFallDown(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_FALL_DOWN));
	m_pBaseAction[BaseAction::SitRecover] = dbg_new GunnerSitRecover(m_pPlayer, pConfig->getActionInfo(GUNNER_ACTION_SIT_RECOVER));
	

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

void ActionMgr::update(float dt) {

	if (m_pRunningAction)
		m_pRunningAction->onUpdate(dt);
}

void ActionMgr::onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyPressed(controller, keyCode);
}

void ActionMgr::onKeyReleased(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyReleased(controller, keyCode);
}

void ActionMgr::onKeyPressedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyPressedBefore(controller, keyCode);
}

void ActionMgr::onKeyReleasedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyReleasedBefore(controller, keyCode);
}

void ActionMgr::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pRunningAction)
		m_pRunningAction->onFrameBegin(animation, texture);
}

void ActionMgr::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pRunningAction)
		m_pRunningAction->onFrameEnd(animation, texture);
}

void ActionMgr::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pRunningAction)
		m_pRunningAction->onAnimationBegin(animation, texture);
}

void ActionMgr::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	if (m_pRunningAction)
		m_pRunningAction->onAnimationEnd(animation, texture);
}


bool ActionMgr::canRunAction() {

	if (m_pRunningAction) {

		if (m_pRunningAction->isForceCancelable())
			return true;

		return false;	// 액션을 수행중인데 캔슬 가능한 경우가 아닌 경우
	}

	return true;
}

bool ActionMgr::isBaseActionRunning(BaseAction_t baseActionType) {
	if (m_pBaseAction[baseActionType] == nullptr)
		return false;

	return m_pBaseAction[baseActionType] == m_pRunningAction;
}

bool ActionMgr::isActionRunning(int actionCode) {
	DebugAssertMsg(m_ActionMap.Exist(actionCode), "해당 코드값의 액션이 존재하지 않습니다.");

	if (m_ActionMap[actionCode] != m_pRunningAction)
		return false;

	return true;
}

bool ActionMgr::isRunnningActionIdleOrWalkAction() {
	if (m_pRunningAction == nullptr)
		return true;

	if (m_pRunningAction == m_pBaseAction[BaseAction::Idle] ||
		m_pRunningAction == m_pBaseAction[BaseAction::Walk])
		return true;

	return false;
}


SGAction* ActionMgr::getAction(int actionCode) {
	DebugAssertMsg(m_ActionMap.Exist(actionCode), "해당 코드값의 액션이 존재하지 않습니다.");
	return m_ActionMap.Get(actionCode);
}

SGAction* ActionMgr::getBaseAction(BaseAction_t baseActionType) {
	DebugAssertMsg(m_pBaseAction[baseActionType], "해당 기초액션이 아직 구현되지 않았습니다.");
	return m_pBaseAction[baseActionType];
}

int Count = 0;

void ActionMgr::runAction(int actionCode) {
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

void ActionMgr::runAction(SGAction* action) {
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

void ActionMgr::runBaseAction(BaseAction_t baseActionType) {
	DebugAssertMsg(m_pBaseAction[baseActionType], "해당 기초액션이 아직 구현되지 않았습니다.");
	runAction(m_pBaseAction[baseActionType]);
}

void ActionMgr::stopActionForce() {
	if (m_pRunningAction != nullptr) {
		m_pRunningAction->onActionEnd();
	}
	m_pPreviousAction = m_pRunningAction;
	m_pRunningAction = nullptr;
}

