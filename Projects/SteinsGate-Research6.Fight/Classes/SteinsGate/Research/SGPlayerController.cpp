/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 8:59:10 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGPlayerController.h"

#include <SteinsGate/Research/SGHostPlayer.h>
#include <SteinsGate/Research/SGAction.h>
#include <SteinsGate/Research/SGMapLayer.h>
#include <SteinsGate/Research/SGMapInfo.h>

USING_NS_CC;
USING_NS_JC;

SGPlayerController* SGPlayerController::create(SGHostPlayer* player, SGCharacter* character, SGActionManager* actionManager) {
	SGPlayerController* pController = new SGPlayerController(player, character, actionManager);
	pController->init();
	return pController;
}


SGPlayerController::SGPlayerController(SGHostPlayer* player, SGCharacter* character, SGActionManager* actionManager)
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
	m_ControlKeyPressedMap[pressedKey] = true;
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
	m_ControlKeyPressedMap[releasedKey] = false;
	m_LastestReleasedKey.ControlKey = releasedKey;
	m_LastestReleasedKey.Time = DateTime::NowDetail();
}

SpriteDirection_t SGPlayerController::getSpriteDirection() {
	return m_pCharacter->getActorSprite()->getSpriteDirection();
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
	if (pRunningAction == nullptr)
		return;

	// 좌,우 | 위,아래 독립적으로 계산해야함
	// 한개로만 하게되면 예를들어 Left, Down을 동시에 눌렀을 때
	// thickness.origin.x -= fSpeedX 적용된 값이 Down 체크시에도 적용되어버려서
	// 이 적용된 값 때문에 Down에서 lb, rb 충돌 체크가 항상 참이 되어버림
	// --------------------------------------------------------------
	//  23/01/28 -> 좌,우,위,아래 모두 독립적으로 가능하도록 추가
	SGRect thicknessPosLR = m_pCharacter->getThicknessBoxRect();
	SGRect thicknessPosUD = thicknessPosLR;

	float fSpeedX = pRunningAction->getMoveSpeedX() / 60.0f;

	if (isKeyPressed(ControlKey::Left) && pRunningAction->isMoveableNegativeX()) {
		thicknessPosLR.origin.x -= fSpeedX;
		updateLeftMove(pMapLayer, pMapInfo, thicknessPosLR);

	} else if (isKeyPressed(ControlKey::Right) && pRunningAction->isMoveablePositiveX()) {
		thicknessPosLR.origin.x += fSpeedX;
		updateRightMove(pMapLayer, pMapInfo, thicknessPosLR);

	}

	float fSpeedY = pRunningAction->getMoveSpeedY() / 60.0f;

	if (isKeyPressed(ControlKey::Up) && pRunningAction->isMoveablePositiveY()) {
		thicknessPosUD.origin.y += fSpeedY;
		updateUpMove(pMapLayer, pMapInfo, thicknessPosUD);

	} else if (isKeyPressed(ControlKey::Down) && pRunningAction->isMoveableNegativeY()) {
		thicknessPosUD.origin.y -= fSpeedY;
		updateDownMove(pMapLayer, pMapInfo, thicknessPosUD);

	}
}

void SGPlayerController::updateLeftMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo,  const SGRect& thicknessRect) {
	SGVec2 lb{ thicknessRect.origin.x, thicknessRect.origin.y };
	SGVec2 lt{ thicknessRect.origin.x, thicknessRect.origin.y + thicknessRect.size.height };

	// lb, lt 체크
	if (mapInfo->checkWall(lb) || mapInfo->checkWall(lt) || mapLayer->isCollideWithObstacles(thicknessRect))
		return;

	m_pCharacter->setPositionRealX(thicknessRect.origin.x);
}


void SGPlayerController::updateRightMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect) {
	SGVec2 rb{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y };
	SGVec2 rt{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y + thicknessRect.size.height };

	// rb, rt 체크
	if (mapInfo->checkWall(rb) || mapInfo->checkWall(rt) || mapLayer->isCollideWithObstacles(thicknessRect))
		return;

	m_pCharacter->setPositionRealX(thicknessRect.origin.x);
}

void SGPlayerController::updateUpMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect) {
	SGVec2 lt{ thicknessRect.origin.x, thicknessRect.origin.y + thicknessRect.size.height };
	SGVec2 rt{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y + thicknessRect.size.height };

	// lt, rt 체크
	if (mapInfo->checkWall(lt) || mapInfo->checkWall(rt) || mapLayer->isCollideWithObstacles(thicknessRect))
		return;

	m_pCharacter->setPositionRealY(thicknessRect.origin.y);
}

void SGPlayerController::updateDownMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect) {
	SGVec2 lb{ thicknessRect.origin.x, thicknessRect.origin.y };
	SGVec2 rb{ thicknessRect.origin.x + thicknessRect.size.width, thicknessRect.origin.y };

	// lb, rb 체크
	if (mapInfo->checkWall(lb) || mapInfo->checkWall(rb) || mapLayer->isCollideWithObstacles(thicknessRect))
		return;

	m_pCharacter->setPositionRealY(thicknessRect.origin.y);
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
	SGVector<ControlKey_t> vPressed{ Direction::Max };

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
