/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:10 AM
 * =====================
 *
 */

#include "PlayerController.h"

#include <SteinsGate/Research/Player.h>

// ActionAbstract* pRunningAction = m_pActionManager->getRunningAction();

using namespace cocos2d;

using namespace JCore;

PlayerController* PlayerController::create(Player* player, CharacterSprite* character, PlayerActionManager* actionManager) {
	PlayerController* pController = new PlayerController(player, character, actionManager);
	pController->init();
	pController->autorelease();
	return pController;
}

PlayerController::~PlayerController() {
	Log("플레이어 컨트롤러 소멸\n");
}

void PlayerController::init() {
	for (int i = 0; i < int(cocos2d::EventKeyboard::KeyCode::MAX); i++) {
		m_CocosKeyCodeToControlKeyMap[i] = ControlKey::None;
	}

	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW)] = ControlKey::Left;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)] = ControlKey::Right;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)] = ControlKey::Up;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW)] = ControlKey::Down;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_X)] = ControlKey::Attack;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_C)] = ControlKey::Jump;
	m_bCabUseCommand = true;
}

void PlayerController::update(float delta) {
	updateWalking(delta);
	m_pActionManager->update(delta);
}

void PlayerController::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	ControlKey_t ePressedKey = m_CocosKeyCodeToControlKeyMap[int(keyCode)];
	if (ePressedKey == ControlKey::None) return;
	m_pActionManager->onKeyPressedBefore(this, keyCode);
	onKeyPressed(ePressedKey);
	m_pActionManager->onKeyPressed(this, keyCode);
}
void PlayerController::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	ControlKey_t eReleasedKey = m_CocosKeyCodeToControlKeyMap[int(keyCode)];
	if (eReleasedKey == ControlKey::None) return;
	m_pActionManager->onKeyReleasedBefore(this, keyCode);
	onKeyReleased(eReleasedKey);
	m_pActionManager->onKeyReleased(this, keyCode);
}

void PlayerController::onKeyPressed(ControlKey_t pressedKey) {
	++m_ControlKeyPressedMap[pressedKey];
	DateTime now = DateTime::NowDetail();

	m_LastestPressedKey.ControlKey = pressedKey;;
	m_LastestPressedKey.Time = now;

	reflectPressedMoveKeys();

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
	// [m_ControlKeyPressedMap 업데이트 전 수행해야하는 작업]
	// 왼쪽을 누른 상태에서 오른쪽을 같이 누른 후 왼쪽을 때버린 경우 같은
	if (fixFreezedWalkState(releasedKey)) {
		
	}

	--m_ControlKeyPressedMap[releasedKey];
	m_LastestReleasedKey.ControlKey = releasedKey;
	m_LastestReleasedKey.Time = DateTime::NowDetail();

	// 실행중인 액션이 있는 경우 모션 전환이 있어서는 안된다.
	if (hasRunningAction()) {
		return;
	}

	if (!isMoveKeyPressed()) {
		idle();
	}
}

bool PlayerController::isKeyPressed(ControlKey_t controlKey) {
	return m_ControlKeyPressedMap[int(controlKey)] > 0;
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

bool PlayerController::isHorizontalKeyPressed() {
	if (isKeyPressed(ControlKey::Left) &&
		isKeyPressed(ControlKey::Right))
		return true;
	return false;
}

bool PlayerController::isVerticalKeyPressed() {
	if (isKeyPressed(ControlKey::Up) &&
		isKeyPressed(ControlKey::Down))
		return true;

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

	ComboAction* pAction;

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
		Log("[콤보 실행] %s\n", pAction->getComboKeys().String().Source());
		m_pActionManager->runAction(pAction);
	}
}

void PlayerController::fixReverseKeyPressed(ControlKey_t pressedKey)
{
	// 왼쪽 눌린 상태에서 오른쪽 눌린 경우 멈추도록 함.
	if (isKeyPressed(ControlKey::Left) && pressedKey == ControlKey::Right ||
		isKeyPressed(ControlKey::Right) && pressedKey == ControlKey::Left) {
		idle();
	}
}

bool PlayerController::isFreezedWalkState(ControlKey_t releasedKey) {
	if (m_pActionManager->hasRunningAction())
		return false;

	if (isKeyPressed(ControlKey::Left) && isKeyPressed(ControlKey::Right) &&
		(releasedKey == ControlKey::Left || releasedKey == ControlKey::Right)) {
		return true;
	}

	if (isKeyPressed(ControlKey::Up) && isKeyPressed(ControlKey::Down) &&
		(releasedKey == ControlKey::Up || releasedKey == ControlKey::Down)) {
		walk();
		return true;
	}

	return false;
}


bool PlayerController::fixFreezedWalkState(ControlKey_t releasedKey) {

	if (hasRunningAction())
		return false;

	ControlKey_t eReverseKey = ControlKey::ReverseDirection[releasedKey];

	if (isKeyPressed(ControlKey::Left) && isKeyPressed(ControlKey::Right) &&
		(releasedKey == ControlKey::Left || releasedKey == ControlKey::Right)) {
		updateDirection(eReverseKey);
		walk();
		return true;
	}

	if (isKeyPressed(ControlKey::Up) && isKeyPressed(ControlKey::Down) &&
		(releasedKey == ControlKey::Up || releasedKey == ControlKey::Down)) {
		walk();
		return true;
	}

	return false;
}

void PlayerController::idle() {
	m_pCharacter->setIdle();
}

void PlayerController::walk() {	
	m_pCharacter->setWalk();
}

void PlayerController::updateWalking(float dt) {
	ActionAbstract* pRunningAction = m_pActionManager->getRunningAction();
	// 액션중 이동가능한 액션인 경우 해당 액션의 이동속도로 움직일 수 있도록 한다.
	const float fSpeedX = pRunningAction ? pRunningAction->getMoveSpeed() : DefaultMoveSpeedPerFPSX_v;

	if (pRunningAction && pRunningAction->isMoveable() == false)
		return;
	
	if (isKeyPressed(ControlKey::Left)) {
		m_pActor->setPositionX(m_pActor->getPositionX() - fSpeedX);
	}

	if (isKeyPressed(ControlKey::Right)) {
		m_pActor->setPositionX(m_pActor->getPositionX() + fSpeedX);
	}

	if (isKeyPressed(ControlKey::Up)) {
		m_pActor->setPositionY(m_pActor->getPositionY() + DefaultMoveSpeedPerFPSY_v);
	}

	if (isKeyPressed(ControlKey::Down)) {
		m_pActor->setPositionY(m_pActor->getPositionY() - DefaultMoveSpeedPerFPSY_v);
	}
}

void PlayerController::updateDirection(ControlKey_t pressedKey) {

	if (hasRunningAction())
		return;

	// 방향전환 가능 여부
	if (pressedKey == ControlKey::Right) {
		m_pCharacter->setForwardDirection();
		m_eDir = SpriteDirection::Right;
	} else if (pressedKey == ControlKey::Left) {
		m_pCharacter->setBackwardDirection();
		m_eDir = SpriteDirection::Left;
	}
}


// 1. 스킬시전 중 꾹 누른키에 대한 처리를 액션이 끝난 후 마저 처리해줘야한다.
// 2. 액션이 없는 경우 키 입력시마다 반영해줘야한다.
void PlayerController::reflectPressedMoveKeys() {
	// 액션 수행중 방향키로 움직일 수 있는 액션인 경우라도 걷기 나 평상시 애니메이션이 실행되어선 안된다.

	if (hasRunningAction())
		return;

	// 방향키만 읽음, StaticVector 변경 필요
	JCore::Vector<ControlKey_t> vPressed{ Direction::Max }; 

	for (int i = 0; i < Direction::Max; ++i) {
		if (isKeyPressed(ControlKey_t(i)))
			vPressed.PushBack(ControlKey_t(i));
	}

	if (vPressed.Size() == 0) {
		idle();
		return;
	}

	// 동시 키입력 소거
	// 남은건 대각이동 또는 한가지 방향밖에 없다.
	if (vPressed.Exist(ControlKey::Left) && vPressed.Exist(ControlKey::Right)) {
		vPressed.Remove(ControlKey::Left);
		vPressed.Remove(ControlKey::Right);
	}

	if (vPressed.Exist(ControlKey::Up) && vPressed.Exist(ControlKey::Down)) {
		vPressed.Remove(ControlKey::Up);
		vPressed.Remove(ControlKey::Down);
	}

	if (vPressed.Exist(ControlKey::Right))
		updateDirection(ControlKey::Right);
	else if (vPressed.Exist(ControlKey::Left))
		updateDirection(ControlKey::Left);

	walk();
}

ControlKey_t PlayerController::convertControlKey(cocos2d::EventKeyboard::KeyCode keyCode) {
	return m_CocosKeyCodeToControlKeyMap[int(keyCode)];
}
