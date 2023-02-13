/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGGunnerJump.h"

#include <SteinsGate/Client/SGHostPlayer.h>
#include <SteinsGate/Client/SGAnimationDefine.h>
#include <SteinsGate/Client/SGProjectileDefine.h>
#include <SteinsGate/Client/SGActionDefine.h>
#include <SteinsGate/Client/SGCharBaseInfo.h>

#define MinimumShotHeight 30.0f

SGGunnerJump::SGGunnerJump(SGHostPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo) {}

// #define GUNNER_ANIMATION_JUMP_START					14
// #define GUNNER_ANIMATION_JUMP_UP						15
// #define GUNNER_ANIMATION_JUMP_DOWN					16
// #define GUNNER_ANIMATION_JUMP_END					17
// 
// #define GUNNER_ANIMATION_JUMP_SHOT_BEGIN				18
// #define GUNNER_ANIMATION_JUMP_SHOT_SHOT				19
// #define GUNNER_ANIMATION_JUMP_SHOT_END				20

void SGGunnerJump::onActionBegin() {
	SGActionManager* pActionManager = m_pPlayer->getActionManager();
	SGAction* pPrevious = pActionManager->getPreviousAction();

	setMoveable(false);

	m_bJumpUpbegin = false;
	m_bJumpDownBegin = false;
	m_bCanFire = false;
	m_bFireMode = false;
	m_bWaitForFire = false;
	m_bRightFire = false;
	m_iChargedShotCount = 0;
	m_iShotCount = 0;
	m_iMaxShotCount = m_pBaseInfo->JumpShotCount[m_eWeaponType];

	if (pPrevious->getActionCode() == GUNNER_ACTION_RUN) {
		m_fMoveSpeedFPSX = pPrevious->getMoveSpeedX();
		m_fMoveSpeedFPSY = pPrevious->getMoveSpeedY();
	} else {
		SGAction* pWalkAction = pActionManager->getBaseAction(BaseAction::Walk);
		m_fMoveSpeedFPSX = pWalkAction->getMoveSpeedX();
		m_fMoveSpeedFPSY = pWalkAction->getMoveSpeedY();
	}

	m_pPlayer->runAnimation(GUNNER_ANIMATION_JUMP_START);
}

void SGGunnerJump::onUpdate(float dt) {
	SGCharacter* pCharacter = m_pPlayer->getCharacter();

	updateJumpUp(pCharacter, dt);
	updateJumpDown(pCharacter, dt);
}

void SGGunnerJump::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	int iAnimationCode = animation->getAnimationCode();
	SGCharacter* pCharacter = m_pPlayer->getCharacter();
	SGPlayerController* pController = m_pPlayer->getController();

	// 착지 경지 효과를 위해 움직임 봉인
	if (iAnimationCode == GUNNER_ANIMATION_JUMP_END) {
		setMoveable(false);
	} 
}

void SGGunnerJump::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	int iAnimationCode = animation->getAnimationCode();
	SGCharacter* pCharacter = m_pPlayer->getCharacter();

	if (iAnimationCode == GUNNER_ANIMATION_JUMP_START) {
		setMoveable(true);

		m_bJumpUpbegin = true;
		m_bCanFire = true;

		pCharacter->runAnimation(GUNNER_ANIMATION_JUMP_UP);
		pCharacter->addForceY(pCharacter->getBaseInfo()->JumpForce);
	} else if (iAnimationCode == GUNNER_ANIMATION_JUMP_SHOT_BEGIN) {
		shot(pCharacter);
	} else if (iAnimationCode == GUNNER_ANIMATION_JUMP_END) {
		stop();
	} 
}

void SGGunnerJump::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	
}

void SGGunnerJump::onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	int iAnimationCode = animation->getAnimationCode();
	if (iAnimationCode != GUNNER_ANIMATION_JUMP_SHOT_SHOT) {
		return;
	}

	SGCharacter* pCharacter = m_pPlayer->getCharacter();
	int iFrameIndexInAnimation = animation->getFrameIndexInAnimation();

	// jump_shot_shot 73번 인덱스 호출완료시
	if (iFrameIndexInAnimation == 1) {

		// 쏘고 나서 높이가 0인경우 바로 정지시켜주도록 하자.
		if (pCharacter->getPositionActorY() == 0) {
			pCharacter->runAnimation(GUNNER_ANIMATION_JUMP_END);
			m_bJumpDownBegin = false;
			return;
		}

		// 높이가 그래도 어느정도 있어야 쏠 수 잇도록 하자.
		if (pCharacter->getPositionActorY() >= MinimumShotHeight && !shot(pCharacter)) {

			// 시간내로 총을 못쏜 경우에대한 처리, 공중에선 쏠 수 있는 횟수만 여유롭다면 언제든지 마음대로 계속 총을 쏠 수 있다.
			m_bWaitForFire = true; 
		}
	}
}

void SGGunnerJump::onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	ControlKey_t controlKey = controller->convertControlKey(keyCode);
	SGCharacter* pCharacter = m_pPlayer->getCharacter();

	if (!m_bFireMode)
		controller->updateDirection(controlKey);

	if (controlKey != ControlKey::Attack)
		return;

	if (!m_bCanFire)
		return;

	++m_iChargedShotCount;

	if (!m_bFireMode) {
		pCharacter->runAnimation(GUNNER_ANIMATION_JUMP_SHOT_BEGIN);
		m_bFireMode = true;
	}
	
	if (m_bWaitForFire && pCharacter->getPositionActorY() >= MinimumShotHeight) {
		shot(pCharacter);
		m_bWaitForFire = false;
	}
}

void SGGunnerJump::updateJumpUp(SGCharacter* character, float dt) {
	if (!m_bJumpUpbegin)
		return;

	// Step 1. 상승 중
	if (m_bJumpUpbegin && character->getDownTime() > 0.0f) {
		m_bJumpUpbegin = false;
		m_bJumpDownBegin = true;

		if (!m_bFireMode) {
			character->runAnimation(GUNNER_ANIMATION_JUMP_DOWN);
		}
	}
}
void SGGunnerJump::updateJumpDown(SGCharacter* character, float dt) {
	if (!m_bJumpDownBegin)
		return;

	// Step 2. 하강 중
	if (m_bJumpDownBegin && character->isOnTheGround()) {
		character->runAnimation(GUNNER_ANIMATION_JUMP_END);
		m_bJumpDownBegin = false;
	}
}


bool SGGunnerJump::shot(SGCharacter* character) {
	if (m_iChargedShotCount <= 0) 
		return false;

	if (m_iShotCount >= m_iMaxShotCount)
		return false;

	m_iShotCount++;
	--m_iChargedShotCount;
	m_bRightFire = !m_bRightFire;		// 좌/우 토글
	character->runAnimation(GUNNER_ANIMATION_JUMP_SHOT_SHOT);
	
	reboundX(character);			// X축 반동
	reboundY(character);			// Y축 반동
	createBullet();

	return true;
}


void SGGunnerJump::reboundX(SGCharacter* character) {
	
	SpriteDirection_t eDirection = character->getSpriteDirection();

	// 반동 초기화
	character->removeForceX();

	// 1. 우측키를 누른 상태
	switch (eDirection) {
	case SpriteDirection::Right:	reboundXLeft(character);	break;	// 우측으로 바라보고 있으면 좌측 반동
	case SpriteDirection::Left:		reboundXRight(character);	break;	// 좌측으로 바라보고 있으면 우측 반동
	default: DebugAssertMsg(false, "방향이 도대체 어디에요?");
	}
}

void SGGunnerJump::reboundXLeft(SGCharacter* character) {
	SGPlayerController* pController = m_pPlayer->getController();


	/*
	 * 생각 정리.
	 * 1. 우측으로 총을 쏠때 우측키를 누른 상태면 아무 것도 없음
	 * 2. 우측으로 총을 쏠때 아무키를 안누른 상태면 X 반동만 있어야함
	 * 3. 우측으로 총을 쏠때 왼쪽 키를 누른 상태면 X 반동 + 이동속도 추가
	 * 코드로 옮기자.
	 */

	// 우측으로 한번이라도 쏜 이후부터는 우측 방향 이동 불가능
	m_bMoveablePositiveX = false;
	m_fMoveSpeedFPSX = 0.0f;

	if (pController->isKeyPressed(ControlKey::Right)) {
		return;
	}

	if (!pController->isMoveKeyPressed()) {
		character->addForceX(m_pBaseInfo->JumpShotForceX[m_eWeaponType] * -1);
		return;
	}

	if (pController->isKeyPressed(ControlKey::Left)) {
		character->addForceX(m_pBaseInfo->JumpShotForceX[m_eWeaponType] * -1);
		m_fMoveSpeedFPSX = m_pBaseInfo->JumpShotMoveSpeedX[m_eWeaponType];
	}
}

void SGGunnerJump::reboundXRight(SGCharacter* character) {
	SGPlayerController* pController = m_pPlayer->getController();

	m_bMoveableNegativeX = false;
	m_fMoveSpeedFPSX = 0.0f;

	if (pController->isKeyPressed(ControlKey::Left)) {
		return;
	}

	if (!pController->isMoveKeyPressed()) {
		character->addForceX(m_pBaseInfo->JumpShotForceX[m_eWeaponType]);
		return;
	}

	if (pController->isKeyPressed(ControlKey::Right)) {
		character->addForceX(m_pBaseInfo->JumpShotForceX[m_eWeaponType]);
		m_fMoveSpeedFPSX = m_pBaseInfo->JumpShotMoveSpeedX[m_eWeaponType];
	}
}



void SGGunnerJump::reboundY(SGCharacter* character) {
	// 중력 역행
	float fRemovedYForce = character->removeForceY();
	if (fRemovedYForce > 0.0f) {
		character->addForceY(m_pBaseInfo->JumpShotForceY[m_eWeaponType]);
	}

}

void SGGunnerJump::createBullet() {

	// 거너 총 종류에따라 프로젝틸 혹은 히트박스 생성
	FrameEventType_t eFrameEventType = WeaponType::ShotFrameEventType[m_eWeaponType];

	int iFrameEventId = -1;

	if (m_bRightFire) {
		switch (m_eWeaponType) {
		case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_JUMP_RIGHT; break;
		default: DebugAssertMsg(false, "총 종류가 이상합니다. (1)");
		}
	}
	else {
		switch (m_eWeaponType) {
		case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_JUMP_LEFT; break;
		default: DebugAssertMsg(false, "총 종류가 이상합니다. (2)");
		}
	}

	DebugAssertMsg(iFrameEventId != -1, "프레임 이벤트 ID가 설정되지 않았습니다.");
	runFrameEvent(eFrameEventType, iFrameEventId);
}
