/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:10 AM
 * =====================
 *
 */

#include "PlayerController.h"

using namespace cocos2d;

using namespace JCore;

PlayerController* PlayerController::create(CharacterSprite* character) {
	PlayerController* pController = new PlayerController(character);
	pController->init();
	pController->autorelease();
	return pController;
}

bool PlayerController::init() {
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW)] = ControlKey::Left;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)] = ControlKey::Right;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)] = ControlKey::Up;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW)] = ControlKey::Down;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_X)] = ControlKey::Attack;
	m_CocosKeyCodeToControlKeyMap[int(cocos2d::EventKeyboard::KeyCode::KEY_C)] = ControlKey::Jump;


	auto spActionRunRight = MakeShared<ComboAction>(
		[this]				{ this->run(Direction::Right); },
		[this](float dt)	{ this->updateRun(dt); },
		"우측 달리기", 
		ComboKeyList{ ControlKey::Right, ControlKey::Right }, 
		true,
		true
	);


	auto spActionRunLeft = MakeShared<ComboAction>(
		[this]				{ this->run(SpriteDirection::Left); },
		[this](float dt)	{ this->updateRun(dt); },
		"좌측 달리기", 
		ComboKeyList{ ControlKey::Left, ControlKey::Left },
		true,
		true
	);

	auto spActionSlidingRight = MakeShared<ComboAction>(
		[this]				{ this->slide(Direction::Right); },
		"우측 슬라이딩", 
		ComboKeyList{ ControlKey::Right, ControlKey::Right, ControlKey::Attack }, 
		true,
		false
	);

	auto spActionSlidingLeft = MakeShared<ComboAction>(
		[this]				{ this->slide(Direction::Left); },
		"좌측 슬라이딩", 
		ComboKeyList{ ControlKey::Left, ControlKey::Left, ControlKey::Attack },
		true,
		false
	);

	m_ComboTree.AddComboAction(spActionRunLeft);
	m_ComboTree.AddComboAction(spActionRunRight);
	m_ComboTree.AddComboAction(spActionSlidingRight);
	m_ComboTree.AddComboAction(spActionSlidingLeft);
	return true;
}

void PlayerController::update(float delta) {
	bool bMoveable = true;
	if (m_spRunningAction != nullptr && m_spRunningAction->IsDoing()) {
		bMoveable = m_spRunningAction->IsMoveable();
		m_spRunningAction->CallUpdate(delta);
	} 

	if (bMoveable && !m_bPreventMove)
		updateWalking(delta);

}

void PlayerController::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	ControlKey_t ePressedKey = m_CocosKeyCodeToControlKeyMap[int(keyCode)];
	if (ePressedKey == ControlKey::None) return;
	onKeyPressed(ePressedKey);

}
void PlayerController::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	ControlKey_t eReleasedKey = m_CocosKeyCodeToControlKeyMap[int(keyCode)];
	if (eReleasedKey == ControlKey::None) return;
	onKeyReleased(eReleasedKey);
}

void PlayerController::onKeyPressed(ControlKey_t pressedKey) {
	++m_ControlKeyPressedMap[pressedKey];
	DateTime now = DateTime::NowDetail();

	m_eLastestPressedKey.ControlKey = pressedKey;;
	m_eLastestPressedKey.Time = now;

	reflectPressedMoveKeys();

	if (m_bPreventMove)
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
	checkFreezeWalkState(releasedKey);
	--m_ControlKeyPressedMap[releasedKey];
	m_eLastestReleasedKey.ControlKey = releasedKey;

	if (m_bPreventMove) {
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
	if (!isKeyPressed(ControlKey::Left) &&
		!isKeyPressed(ControlKey::Right) &&
		!isKeyPressed(ControlKey::Up) &&
		!isKeyPressed(ControlKey::Down)) {
		return false;
	}
	return true;
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

	ComboActionPtr spAction;

	// 없는 경우 위로 거슬러 올라가서 확인
	// 예를 들어 -> -> X를 입력했는데 키가 바인딩 되어있지 않으면
	// 음..		-> -> 를 검사하고 키가 바인딩 되어있지 않으면
	//          ->    를 검사한다.
	// 
	// ↑ -> -> 키를 순서대로 입력했을 때
	//   -> -> 키가 바인딩 되어있음에도 불구하고 실행 안되는 경우를 방지하기 위함
	while ((spAction = m_ComboTree.GetComboAction(sequence)) == nullptr && iSequenceCount > 0) {
		sequence[--iSequenceCount] = ControlKey::None;
	}


	if (spAction.Exist()) {

		ComboAction* pAction = spAction.GetPtr();
		ComboAction* pRunningAction = m_spRunningAction.GetPtr();

		if (pRunningAction != nullptr &&
			pRunningAction->IsDoing()) {

			if (pRunningAction->IsCancelable()) {
				pRunningAction->CallCancel();
				pRunningAction = nullptr;
			} 
			else 
				return;	// 액션을 수행중인데 캔슬 가능한 경우가 아니면 그냥 끝낸다.
		}

		m_spRunningAction = spAction;
		m_spRunningAction->CallRun();
	}
}

void PlayerController::checkReverseKeyPressed(ControlKey_t pressedKey)
{
	// 왼쪽 눌린 상태에서 오른쪽 눌린 경우 멈추도록 함.
	if (isKeyPressed(ControlKey::Left) && pressedKey == ControlKey::Right ||
		isKeyPressed(ControlKey::Right) && pressedKey == ControlKey::Left) {
		idle();
	}
}

void PlayerController::checkFreezeWalkState(ControlKey_t releasedKey) {

	if (m_bPreventMove)
		return;

	ControlKey_t eReverseKey =ControlKey::ReverseDirection[releasedKey];

	if (isKeyPressed(ControlKey::Left) && isKeyPressed(ControlKey::Right)) {
		updateDirection(ControlKey::ReverseDirection[releasedKey]);
		
		if (!isRunning())
			walk();
	}

	if (isKeyPressed(ControlKey::Up) && isKeyPressed(ControlKey::Down) &&
		(releasedKey == ControlKey::Up || releasedKey == ControlKey::Down)) {

		if (!isRunning())
			walk();
	}

}

void PlayerController::idle() {
	m_eState = State::Idle;
	m_pCharacter->setIdle();
	m_bPreventMove = false;
}

void PlayerController::walk() {	
	if (m_bPreventMove)
		return;

	if (isRunning())
		return;

	m_pCharacter->setWalk();
	m_eState = State::Walk;
}

void PlayerController::updateWalking(float dt) {

	const float fSpeedX = isWalking() ? DefaultMoveSpeedPerFPSX_v : DefaultRunSpeedPerFPSX_v;
	
	if (isIdle())
		return;

	if (isKeyPressed(ControlKey::Left)) {
		m_pCharacter->setPositionX(m_pCharacter->getPositionX() - fSpeedX);
	}

	if (isKeyPressed(ControlKey::Right)) {
		m_pCharacter->setPositionX(m_pCharacter->getPositionX() + fSpeedX);
	}

	if (isKeyPressed(ControlKey::Up)) {
		m_pCharacter->setPositionY(m_pCharacter->getPositionY() + DefaultMoveSpeedPerFPSY_v);
	}

	if (isKeyPressed(ControlKey::Down)) {
		m_pCharacter->setPositionY(m_pCharacter->getPositionY() - DefaultMoveSpeedPerFPSY_v);
	}
}

void PlayerController::run(SpriteDirection_t direction) {
	m_eState = State::Run;
	m_pCharacter->setRunning();
}

void PlayerController::updateRun(float dt) {
}


void PlayerController::slide(SpriteDirection_t direction) {
	m_eState = State::Sliding;
	m_bPreventMove = true;

	m_pCharacter->setSlide(direction,
		CallFunc::create([this]() {}),
		CallFunc::create([this]() { 
			m_bPreventMove = false; 
			m_spRunningAction->Done();
			reflectPressedMoveKeys(); 
		})
	);
}


void PlayerController::updateDirection(ControlKey_t pressedKey) {

	if (m_bPreventMove)
		return;

	// 방향전환 가능 여부
	if (pressedKey == ControlKey::Right) {
		m_pCharacter->setForwardDirection();
	} else if (pressedKey == ControlKey::Left) {
		m_pCharacter->setBackwardDirection();
	}
}


void PlayerController::reflectPressedMoveKeys() {
	if (m_bPreventMove)
		return;

	JCore::Vector<ControlKey_t> vPressed{ Direction::Max }; // 방향키만 읽음

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
