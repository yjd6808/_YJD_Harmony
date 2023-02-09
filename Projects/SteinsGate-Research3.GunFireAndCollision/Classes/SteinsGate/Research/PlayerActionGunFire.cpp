/*
 * 작성자: 윤정도
 * 생성일: 1/15/2023 1:01:00 AM
 * =====================
 *
 */


#include "PlayerActionGunFire.h"

#include <SteinsGate/Research/Player.h>

using namespace cocos2d;


PlayerActionGunFire* PlayerActionGunFire::createRetain() {
	auto action = new PlayerActionGunFire();

	if (action->init()) {
		action->autorelease();
		action->retain();
		return action;
	}

	DebugAssertMessage(false, "초기화에 실패하였습니다.");
	CC_SAFE_DELETE(action);
	return nullptr;
}

bool PlayerActionGunFire::init() {
	// 고정 초기화
	initWithKeys({ ControlKey::Attack });

	m_bMoveable = false;
	m_bShotEnd = false;
	m_bDownShotKeyPressedFirst = false;
	m_bDownShotKeyPressed = false;
	m_bNextFire = false;
	m_bNextFireCheck = false;
	m_bCancelable = false;
	

	// 유동 초기화
	// 플레이어 무기 타입, 공격 속도 보정 고려서해서 애니메이션 프레임 수정 필요
	m_iRightShotCount = 4;

	return true;
}

void PlayerActionGunFire::onActionBegin() {
	PlayerController* pController = m_pPlayer->getController();
	CharacterSprite* pCharacterSprite = m_pPlayer->getCharacterSprite();

	if (pController->isKeyPressed(ControlKey::Down)) {
		m_bDownShotKeyPressed = true;
		pCharacterSprite->runMotion(MotionState::ShotRightDownBegin);
	} else {
		pCharacterSprite->runMotion(MotionState::ShotRightBegin);
	}
}

void PlayerActionGunFire::onActionEnd() {
	auto controller = m_pPlayer->getController();
	
}

void PlayerActionGunFire::onAnimateBegin(
	CharacterSprite* character,
	CharacterPartAnimate* animate,
	CharacterPartSpriteFrame* frame) {

	if (animate->getPartType() != CharacterPartType::Weapon)
		return;
	
	const int iMotionState = animate->getMotionState();
	if (iMotionState == MotionState::ShotRightBegin		||
		iMotionState == MotionState::ShotRightDownBegin ||
		iMotionState == MotionState::ShotLeftBegin		||
		iMotionState == MotionState::ShotLeftDownBegin) {
		
		// 팔뚝 치기
		// m_pPlayer->

	} else if ( iMotionState == MotionState::ShotRightShot ||
				iMotionState == MotionState::ShotRightDownShot /*||
				iMotionState == MotionState::ShotRightEnd ||
				iMotionState == MotionState::ShotRightDownEnd*/) {
		m_bNextFireCheck = true;
	}

	Log("%s 시작\n", MotionState::Name[iMotionState]);
}

void PlayerActionGunFire::onAnimateEnd(
	CharacterSprite* character, 
	CharacterPartAnimate* animate, 
	CharacterPartSpriteFrame* frame) {

	if (animate->getPartType() != CharacterPartType::Weapon) 
		return;

	const int iMotionState = animate->getMotionState();

	if (iMotionState == MotionState::ShotRightBegin ||
		iMotionState == MotionState::ShotRightDownBegin) {
		if (m_bDownShotKeyPressedFirst) {
			character->runMotion(MotionState::ShotRightDownShot);
		} else {
			character->runMotion(MotionState::ShotRightShot);
		}

		m_bNextFireCheck = false;
	} else if (iMotionState == MotionState::ShotRightShot) {
		if (m_bNextFire == false) {
			character->runMotion(MotionState::ShotRightEnd);
			Log("%s 오른쪽 쏘기 중지 종료 시퀀스 돌입\n", MotionState::Name[iMotionState]);
			m_bShotEnd = true;
			return;
		}
		Log("%s 오른쪽 쏘기 한번더 시작\n", MotionState::Name[iMotionState]);
		shot(character, iMotionState);
	} else if (iMotionState == MotionState::ShotRightDownShot) {
		if (m_bNextFire == false) {
			m_bShotEnd = true;
			Log("%s 오른쪽 (아래) 총쏘기 중지 종료 시퀀스 돌입\n", MotionState::Name[iMotionState]);
			character->runMotion(MotionState::ShotRightDownEnd);
			return;
		}
		Log("%s 오른쪽 (아래) 쏘기 한번더 시작\n", MotionState::Name[iMotionState]);
		shot(character, iMotionState);
	} else if (iMotionState == MotionState::ShotRightEnd ||
			   iMotionState == MotionState::ShotRightDownEnd) {
		
		if (m_bShotEnd) {
			Log("%s 액션 완료\n", MotionState::Name[iMotionState]);
			stop();
		} else {
			Log("%s 이어서 왼쪽 쏘기 진행\n", MotionState::Name[iMotionState]);
		}
	}
	else if (iMotionState == MotionState::ShotLeftDownEnd ||
			iMotionState == MotionState::ShotLeftEnd) {
		stop();
	}

	Log("%s 종료\n", MotionState::Name[iMotionState]);
}

// 총쏘는건 꾹 누른게아니라 단발성이므로 update에서 처리할 필요가 없다.
void PlayerActionGunFire::onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {
	ControlKey_t key = controller->convertControlKey(keyCode);

	if (!m_bNextFireCheck)
		return;

	if (key == ControlKey::Attack) {
		m_bNextFire = true;
	}
}


void PlayerActionGunFire::onUpdate(float dt) {
	auto controller = m_pPlayer->getController();

	// 방향키 누르고 있는지 여부는 계속 체크
	if (controller->isKeyPressed(ControlKey::Down)) {
		m_bDownShotKeyPressedFirst = true;
		m_bDownShotKeyPressed = true;
		return;
	}

	m_bDownShotKeyPressed = false;
}

void PlayerActionGunFire::shot(CharacterSprite* character, int motionState) {

	// 아직 우측방향으로 쏠 수 있는 경우
	if (m_iRightShotCount > 0) {
		m_iRightShotCount--;
		if (m_bDownShotKeyPressed)
			character->runMotion(MotionState::ShotRightDownShot);
		else
			character->runMotion(MotionState::ShotRightShot);
		m_bNextFire = false;
		return;
	}

	// 우측 방향으로 더이상 못쏘는 경우
	// 반대총으로 쏘도록 한다.
	if (m_bDownShotKeyPressed) {
		if (motionState == MotionState::ShotRightShot)
			character->runMotion(
				MotionState::ShotRightEnd, 
				MotionState::ShotLeftDownBegin, 
				MotionState::ShotLeftDownShot, 
				MotionState::ShotLeftDownEnd
			);
		else if (motionState == MotionState::ShotRightDownShot) {
			character->runMotion(
				MotionState::ShotRightDownEnd,
				MotionState::ShotLeftDownBegin,
				MotionState::ShotLeftDownShot,
				MotionState::ShotLeftDownEnd
			);
		} else {
			Log("이상한 State %s가 들어옴\n", MotionState::Name[motionState]);
		}
	} else {
		if (motionState == MotionState::ShotRightShot)
			character->runMotion(
				MotionState::ShotRightEnd, 
				MotionState::ShotLeftBegin,
				MotionState::ShotLeftShot, 
				MotionState::ShotLeftEnd
			);
		else if (motionState == MotionState::ShotRightDownShot) {
			character->runMotion(
				MotionState::ShotRightDownEnd,
				MotionState::ShotLeftBegin,
				MotionState::ShotLeftShot,
				MotionState::ShotLeftEnd
			);
		} else {
			Log("이상한 State %s가 들어옴\n", MotionState::Name[motionState]);
		}
	}

	
}


void PlayerActionGunFire::onFrameBegin(
	CharacterSprite* character,
	CharacterPartAnimate* animate,
	CharacterPartSpriteFrame* frame) {

	if (animate->getPartType() != CharacterPartType::Weapon)
		return;

	if (!frame->hasEvent()) {
		return;
	}


	if (frame->getFrameIndex() == MotionStateBegin::ShotDownShot + 1 ||
		frame->getFrameIndex() == MotionStateBegin::ShotOtherDownShot + 1 ||
		frame->getFrameIndex() == MotionStateBegin::ShotShot + 1 ||
		frame->getFrameIndex() == MotionStateBegin::ShotOtherShot + 1) {

		// 
		// m_pPlayer->fire();
	}

	const MotionState_t iMotionState = (MotionState_t)animate->getMotionState();

	if (frame->getFrameIndex() == 26) {
		m_pPlayer->createProjectile(0);
	}

	// 좌
	else if (frame->getFrameIndex() == 35) {
		m_pPlayer->createProjectile(1);
	}

	// 우(아)
	else if (frame->getFrameIndex() == 42) {
		m_pPlayer->createProjectile(2);
	}

	// 좌(아)
	else if (frame->getFrameIndex() == 51) {
		m_pPlayer->createProjectile(3);
	}
}

