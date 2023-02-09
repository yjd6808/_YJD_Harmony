/*
 * 작성자: 윤정도
 * 생성일: 1/14/2023 12:53:49 AM
 * =====================
 *
 */

#include "PlayerActionManager.h"

#include <SteinsGate/Research/World.h>
#include <SteinsGate/Research/Actions.h>


PlayerActionManager* PlayerActionManager::create(Player* player) {
	PlayerActionManager* pManager = new PlayerActionManager(player);
	pManager->init();
	pManager->autorelease();
	return pManager;
}

PlayerActionManager::PlayerActionManager(Player* player)
	: m_pPlayer(player) {}

PlayerActionManager::~PlayerActionManager() {
	m_Map.Values().Extension().ForEach([](ActionAbstract* action) {CC_SAFE_RELEASE(action);});
	Log("액션 매니저 소멸\n");
}

void PlayerActionManager::init() {
	m_pRunningAction = nullptr;

	m_Map.Insert(ActionType::RunLeft, PlayerActionRun::createRetain(SpriteDirection::Left));
	m_Map.Insert(ActionType::RunRight, PlayerActionRun::createRetain(SpriteDirection::Right));
	m_Map.Insert(ActionType::SlidingLeft, PlayerActionSliding::createRetain(SpriteDirection::Left));
	m_Map.Insert(ActionType::SlidingRight, PlayerActionSliding::createRetain(SpriteDirection::Right));
	m_Map.Insert(ActionType::Shot, PlayerActionGunFire::createRetain());
	m_Map.Insert(ActionType::Jump, PlayerActionJump::createRetain());

	m_Map.Values().Extension().ForEach([=](ActionAbstract* action) {
		action->m_pPlayer = m_pPlayer;
		if (action->isComboAction())
			m_ComboTree.AddComboAction((ComboAction*)action);
	});
	
}

void PlayerActionManager::update(float dt) {

	if (m_pRunningAction)
		m_pRunningAction->onUpdate(dt);
}

void PlayerActionManager::onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyPressed(controller, keyCode);
}

void PlayerActionManager::onKeyReleased(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyReleased(controller, keyCode);
}

void PlayerActionManager::onKeyPressedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyPressedBefore(controller, keyCode);
}

void PlayerActionManager::onKeyReleasedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	if (m_pRunningAction)
		m_pRunningAction->onKeyReleasedBefore(controller, keyCode);
}

void PlayerActionManager::onFrameBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) {
	if (m_pRunningAction)
		m_pRunningAction->onFrameBegin(character, animate, frame);
}

void PlayerActionManager::onFrameEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) {
	if (m_pRunningAction)
		m_pRunningAction->onFrameEnd(character, animate, frame);
}

void PlayerActionManager::onAnimateBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) {
	if (m_pRunningAction)
		m_pRunningAction->onAnimateBegin(character, animate, frame);
}

void PlayerActionManager::onAnimateEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame) {
	if (m_pRunningAction)
		m_pRunningAction->onAnimateEnd(character, animate, frame);
}

bool PlayerActionManager::canRunAction() {

	if (m_pRunningAction) {

		if (m_pRunningAction->isCancelable()) 
			return true;
		
		return false;	// 액션을 수행중인데 캔슬 가능한 경우가 아닌 경우
	}

	return true;
}

void PlayerActionManager::setRunningAction(ActionAbstract* action) {
	m_pRunningAction = action;
}

ActionAbstract* PlayerActionManager::getAction(ActionType_t type) {
	DebugAssertMessage(m_Map.Exist(type), "해당 타입의 액션이 존재하지 않습니다.");
	return m_Map.Get(type);
}

void PlayerActionManager::runAction(ActionType_t type) {
	if (!canRunAction())
		return;

	if (m_pRunningAction) {
		m_pRunningAction->stop();
		m_pRunningAction = nullptr;
	}

	m_pPreviousAction = m_pRunningAction;
	m_pRunningAction = getAction(type);
	m_pRunningAction->init();
	m_pRunningAction->play();

	
}

void PlayerActionManager::runAction(ActionAbstract* action) {
	if (!canRunAction())
		return;

	if (m_pRunningAction) {
		Log("현재 액션 캔슬");
		m_pRunningAction->stop();
	}

	m_pPreviousAction = m_pRunningAction;
	m_pRunningAction = action;
	m_pRunningAction->init();
	m_pRunningAction->play();

}


void PlayerActionManager::runComboAction(const ComboKeyList& keys) {
}

