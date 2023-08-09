/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 8:59:10 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "PlayerController.h"

#include <SteinsGate/Common/MapInfo.h>

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/SGAction.h>
#include <SteinsGate/Client/MapLayer.h>
#include <SteinsGate/Client/MoveComponent.h>

USING_NS_CC;
USING_NS_JC;

PlayerController* PlayerController::create(HostPlayer* player, ActionMgr* actionManager) {
	PlayerController* pController = dbg_new PlayerController(player, actionManager);
	pController->init();
	return pController;
}


PlayerController::PlayerController(HostPlayer* player, ActionMgr* actionManager)
	: m_pPlayer(player)
	, m_pActionManager(actionManager)
	, m_vPressedArrowKeyState(4) {
	init();
}

PlayerController::~PlayerController() {
	_LogDebug_("플레이어 컨트롤러 소멸");
}

void PlayerController::init() {
	for (int i = 0; i < int(SGEventKeyboard::KeyCode::MAX); i++) {
		m_CocosKeyCodeToControlKeyMap[i] = ControlKey::None;
	}

	m_CocosKeyCodeToControlKeyMap[int(SGEventKeyboard::KeyCode::KEY_LEFT_ARROW)] = ControlKey::Left;
	m_CocosKeyCodeToControlKeyMap[int(SGEventKeyboard::KeyCode::KEY_RIGHT_ARROW)] = ControlKey::Right;
	m_CocosKeyCodeToControlKeyMap[int(SGEventKeyboard::KeyCode::KEY_UP_ARROW)] = ControlKey::Up;
	m_CocosKeyCodeToControlKeyMap[int(SGEventKeyboard::KeyCode::KEY_DOWN_ARROW)] = ControlKey::Down;
	m_CocosKeyCodeToControlKeyMap[int(SGEventKeyboard::KeyCode::KEY_X)] = ControlKey::Attack;
	m_CocosKeyCodeToControlKeyMap[int(SGEventKeyboard::KeyCode::KEY_C)] = ControlKey::Jump;
	m_bCabUseCommand = true;
}

void PlayerController::update(float delta) {
	updateMove(delta);
}

void PlayerController::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	ControlKey_t ePressedKey = m_CocosKeyCodeToControlKeyMap[int(keyCode)];
	if (ePressedKey == ControlKey::None) return;
	m_pActionManager->onKeyPressedBefore(this, keyCode);
	onKeyPressed(ePressedKey);
	m_pActionManager->onKeyPressed(this, keyCode);
}
void PlayerController::onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	ControlKey_t eReleasedKey = m_CocosKeyCodeToControlKeyMap[int(keyCode)];
	if (eReleasedKey == ControlKey::None) return;
	m_pActionManager->onKeyReleasedBefore(this, keyCode);
	onKeyReleased(eReleasedKey);
	m_pActionManager->onKeyReleased(this, keyCode);
}

void PlayerController::onKeyPressed(ControlKey_t pressedKey) {
	m_ControlKeyPressedMap[pressedKey] = true;
	DateTime now = DateTime::Now();

	m_LastestPressedKey.ControlKey = pressedKey;;
	m_LastestPressedKey.Time = now;

	if (cannotUseCommand())
		return;

	// 덮어쓰기 방지를 위해 뒤에서부터 복사
	for (int i = ComboSequenceCount_v - 2; i >= 0; --i) {
		m_ControlKeySequence[i + 1] = m_ControlKeySequence[i];
	}

	m_ControlKeySequence[0].Time = now;
	m_ControlKeySequence[0].ControlKey = pressedKey;

	checkComboSequence();
}

void PlayerController::onKeyReleased(ControlKey_t releasedKey) {
	m_ControlKeyPressedMap[releasedKey] = false;
	m_LastestReleasedKey.ControlKey = releasedKey;
	m_LastestReleasedKey.Time = DateTime::Now();
}

SpriteDirection_t PlayerController::getSpriteDirection() {
	return m_pPlayer->getActorSprite()->getSpriteDirection();
}

bool PlayerController::isKeyPressed(ControlKey_t controlKey) {
	return m_ControlKeyPressedMap[int(controlKey)];
}

bool PlayerController::isMoveKeyPressed() {
	if (isKeyPressed(ControlKey::Left) ||
		isKeyPressed(ControlKey::Right) ||
		isKeyPressed(ControlKey::Up) ||
		isKeyPressed(ControlKey::Down)) {
		return true;
	}
	return false;
}

void PlayerController::checkComboSequence() {
	ComboKeyList sequence;

	// 우선 현재 입력한 키는 무조건 콤보에 포함되어야하므로
	int iSequenceCount = 1;
	DateTime dtCurrent = m_ControlKeySequence[0].Time;
	sequence[0] = m_ControlKeySequence[0].ControlKey;

	for (int i = 1; i <= ComboSequenceCount_v; ++i, ++iSequenceCount) {
		DateTime& dtBefore = m_ControlKeySequence[i].Time;
		float diff = float(dtCurrent.Diff(dtBefore).GetTotalSeconds());

		if (diff > ComboSequenceDelay_v) {
			break;
		}

		sequence[i] = m_ControlKeySequence[i].ControlKey;
		dtCurrent = dtBefore;
	}

	for (int i = 0; i < iSequenceCount; ++i) {
		sequence[i] = m_ControlKeySequence[i].ControlKey;
	}

	if (iSequenceCount == 0)
		return;

	SGAction* pAction;



	// 없는 경우 위로 거슬러 올라가서 확인
	// 예를 들어 -> -> X를 입력했는데 키가 바인딩 되어있지 않으면
	// 음..		-> -> 를 검사하고 키가 바인딩 되어있지 않으면
	//          ->    를 검사한다.
	// 
	// ↑ -> -> 키를 순서대로 입력했을 때
	//   -> -> 키가 바인딩 되어있음에도 불구하고 실행 안되는 경우를 방지하기 위함
	while ((pAction = m_pActionManager->getComboAction(sequence)) == nullptr && iSequenceCount > 0) {
		sequence[--iSequenceCount] = ControlKey::None;
	}


	if (pAction != nullptr) {
		m_pActionManager->runAction(pAction);
	}
}


void PlayerController::idle() {
	m_pActionManager->runBaseAction(BaseAction::Idle);
}

void PlayerController::walk() {
	m_pActionManager->runBaseAction(BaseAction::Walk);
}

void PlayerController::updateMove(float dt) {

	MoveComponent* pMoveComponent = m_pPlayer->getComponent<MoveComponent>();

	if (pMoveComponent == nullptr)
		return;

	
	SGAction* pRunningAction = m_pActionManager->getRunningAction();

	// 액션중 이동가능한 액션인 경우 해당 액션의 이동속도로 움직일 수 있도록 한다.
	if (pRunningAction == nullptr) {
		pMoveComponent->setSpeed(0, 0);
		return;
	}

	float fSpeedX = 0.0f;
	float fSpeedY = 0.0f;

	if (isKeyPressed(ControlKey::Left) && pRunningAction->isMoveableNegativeX()) {
		fSpeedX = pRunningAction->getMoveSpeedX() / 60.0f * -1;
	} else if (isKeyPressed(ControlKey::Right) && pRunningAction->isMoveablePositiveX()) {
		fSpeedX = pRunningAction->getMoveSpeedX() / 60.0f;
	}

	if (isKeyPressed(ControlKey::Up) && pRunningAction->isMoveablePositiveY()) {
		fSpeedY = pRunningAction->getMoveSpeedY() / 60.0f;
	} else if (isKeyPressed(ControlKey::Down) && pRunningAction->isMoveableNegativeY()) {
		fSpeedY = pRunningAction->getMoveSpeedY() / 60.0f * -1;
	}

	pMoveComponent->setSpeed(fSpeedX, fSpeedY);
}

void PlayerController::updateDirection(ControlKey_t pressedKey) {

	// 방향전환 가능 여부
	if (pressedKey == ControlKey::Right) 
		m_pPlayer->setForwardDirection();
	else if (pressedKey == ControlKey::Left) 
		m_pPlayer->setBackwardDirection();
}


// 1. 스킬시전 중 꾹 누른키에 대한 처리를 액션이 끝난 후 키 입력 상태를 확인해서 상태를 변경해줘야한다. 처리해줘야한다.
// 2. 액션이 없는 경우 키 입력시마다 반영해줘야한다.
void PlayerController::reflectPressedMoveKeys() {
	// 액션 수행중 방향키로 움직일 수 있는 액션인 경우라도 걷기 나 평상시 애니메이션이 실행되어선 안된다.
	// 방향키만 읽음
	m_vPressedArrowKeyState.Clear();

	for (int i = 0; i < Direction::Max; ++i) {
		if (isKeyPressed(ControlKey_t(i)))
			m_vPressedArrowKeyState.PushBack(ControlKey_t(i));
	}

	if (m_vPressedArrowKeyState.Size() == 0) {
		idle();
		return;
	}

	// 동시 키입력 소거
	// 남은건 대각이동 또는 한가지 방향밖에 없다.
	if (m_vPressedArrowKeyState.Exist(ControlKey::Left) && m_vPressedArrowKeyState.Exist(ControlKey::Right)) {
		m_vPressedArrowKeyState.Remove(ControlKey::Left);
		m_vPressedArrowKeyState.Remove(ControlKey::Right);
	}

	if (m_vPressedArrowKeyState.Exist(ControlKey::Up) && m_vPressedArrowKeyState.Exist(ControlKey::Down)) {
		m_vPressedArrowKeyState.Remove(ControlKey::Up);
		m_vPressedArrowKeyState.Remove(ControlKey::Down);
	}

	if (m_vPressedArrowKeyState.Exist(ControlKey::Right))
		updateDirection(ControlKey::Right);
	else if (m_vPressedArrowKeyState.Exist(ControlKey::Left))
		updateDirection(ControlKey::Left);

	walk();
}

ControlKey_t PlayerController::convertControlKey(SGEventKeyboard::KeyCode keyCode) {
	return m_CocosKeyCodeToControlKeyMap[int(keyCode)];
}
