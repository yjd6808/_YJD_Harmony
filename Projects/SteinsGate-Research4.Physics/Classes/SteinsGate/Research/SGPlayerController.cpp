/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 8:59:10 AM
 * =====================
 *
 */

#include "SGPlayerController.h"

#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGComboAction.h>
#include <SteinsGate/Research/SGMapLayer.h>

USING_NS_CC;
USING_NS_JC;

SGPlayerController* SGPlayerController::create(SGPlayer* player, SGCharacter* character, SGActionManager* actionManager) {
	SGPlayerController* pController = new SGPlayerController(player, character, actionManager);
	pController->init();
	return pController;
}


SGPlayerController::SGPlayerController(SGPlayer* player, SGCharacter* character, SGActionManager* actionManager)
	: m_pPlayer(player)
	, m_pCharacter(character)
	, m_pActionManager(actionManager) {
	init();
}

SGPlayerController::~SGPlayerController() {
	Log("플레이어 컨트롤러 소멸\n");
}

void SGPlayerController::init() {
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

void SGPlayerController::update(float delta) {
	updateMove(delta);
}

void SGPlayerController::onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	ControlKey_t ePressedKey = m_CocosKeyCodeToControlKeyMap[int(keyCode)];
	if (ePressedKey == ControlKey::None) return;
	m_pActionManager->onKeyPressedBefore(this, keyCode);
	onKeyPressed(ePressedKey);
	m_pActionManager->onKeyPressed(this, keyCode);
}
void SGPlayerController::onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	ControlKey_t eReleasedKey = m_CocosKeyCodeToControlKeyMap[int(keyCode)];
	if (eReleasedKey == ControlKey::None) return;
	m_pActionManager->onKeyReleasedBefore(this, keyCode);
	onKeyReleased(eReleasedKey);
	m_pActionManager->onKeyReleased(this, keyCode);
}

void SGPlayerController::onKeyPressed(ControlKey_t pressedKey) {
	++m_ControlKeyPressedMap[pressedKey];
	DateTime now = DateTime::NowDetail();

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

void SGPlayerController::onKeyReleased(ControlKey_t releasedKey) {
	--m_ControlKeyPressedMap[releasedKey];
	m_LastestReleasedKey.ControlKey = releasedKey;
	m_LastestReleasedKey.Time = DateTime::NowDetail();
}

bool SGPlayerController::isKeyPressed(ControlKey_t controlKey) {
	return m_ControlKeyPressedMap[int(controlKey)] > 0;
}

bool SGPlayerController::isMoveKeyPressed() {
	if (isKeyPressed(ControlKey::Left) ||
		isKeyPressed(ControlKey::Right) ||
		isKeyPressed(ControlKey::Up) ||
		isKeyPressed(ControlKey::Down)) {
		return true;
	}
	return false;
}

void SGPlayerController::checkComboSequence() {
	SGComboKeyList sequence;

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

	SGComboAction* pAction;

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


void SGPlayerController::idle() {
	m_pActionManager->runBaseAction(BaseAction::Idle);
}

void SGPlayerController::walk() {
	m_pActionManager->runBaseAction(BaseAction::Walk);
}

void SGPlayerController::updateMove(float dt) {

	SGMapLayer* pMapLayer = m_pPlayer->getMapLayer();

	if (pMapLayer == nullptr)
		return;

	SGMapInfo* pMapInfo = pMapLayer->getMapInfo();
	SGAction* pRunningAction = m_pActionManager->getRunningAction();

	// 액션중 이동가능한 액션인 경우 해당 액션의 이동속도로 움직일 수 있도록 한다.
	if (pRunningAction == nullptr || pRunningAction->isMoveable() == false)
		return;

	float fSpeedX = pRunningAction->getMoveSpeedX() / 60.0f;
	float fSpeedY = pRunningAction->getMoveSpeedY() / 60.0f;

	SGRect thicknessPos = m_pCharacter->getThicknessBoxRect();

	Vec2 lb{ thicknessPos.origin };
	Vec2 rt{ thicknessPos.origin.x + thicknessPos.size.width,
			 thicknessPos.origin.y + thicknessPos.size.height };

	if (isKeyPressed(ControlKey::Left)) {
		lb.x -= fSpeedX;
		rt.x -= fSpeedX;

		updateLeftRightMove(pMapInfo, lb, rt);

	} else if (isKeyPressed(ControlKey::Right)) {
		lb.x += fSpeedX;
		rt.x += fSpeedX;

		updateLeftRightMove(pMapInfo, lb, rt);
	}

	if (isKeyPressed(ControlKey::Up)) {
		lb.y += fSpeedY;
		rt.y += fSpeedY;

		updateUpDownMove(pMapInfo, lb, rt);
	} else if (isKeyPressed(ControlKey::Down)) {
		lb.y -= fSpeedY;
		rt.y -= fSpeedY;

		updateUpDownMove(pMapInfo, lb, rt);
	}
}

void SGPlayerController::updateLeftRightMove(
	SGMapInfo* mapInfo, 
	const SGVec2& nextLeftBottomPos, 
	const SGVec2& nextRightTopPos)
{
	if (mapInfo->checkWall(nextLeftBottomPos.x, nextLeftBottomPos.y) ||
		mapInfo->checkWall(nextRightTopPos.x, nextRightTopPos.y))
		return;

	m_pCharacter->setPositionRealX(nextLeftBottomPos.x);
}

void SGPlayerController::updateUpDownMove(
	SGMapInfo* mapInfo, 
	const SGVec2& nextLeftBottomPos, 
	const SGVec2& nextRightTopPos)
{
	if (mapInfo->checkWall(nextLeftBottomPos.x, nextLeftBottomPos.y) ||
		mapInfo->checkWall(nextRightTopPos.x, nextRightTopPos.y))
		return;

	m_pCharacter->setPositionRealY(nextLeftBottomPos.y);
}

void SGPlayerController::updateDirection(ControlKey_t pressedKey) {

	// 방향전환 가능 여부
	if (pressedKey == ControlKey::Right) 
		m_pCharacter->setForwardDirection();
	else if (pressedKey == ControlKey::Left) 
		m_pCharacter->setBackwardDirection();
}


// 1. 스킬시전 중 꾹 누른키에 대한 처리를 액션이 끝난 후 키 입력 상태를 확인해서 상태를 변경해줘야한다. 처리해줘야한다.
// 2. 액션이 없는 경우 키 입력시마다 반영해줘야한다.
void SGPlayerController::reflectPressedMoveKeys() {
	// 액션 수행중 방향키로 움직일 수 있는 액션인 경우라도 걷기 나 평상시 애니메이션이 실행되어선 안된다.
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

ControlKey_t SGPlayerController::convertControlKey(SGEventKeyboard::KeyCode keyCode) {
	return m_CocosKeyCodeToControlKeyMap[int(keyCode)];
}
