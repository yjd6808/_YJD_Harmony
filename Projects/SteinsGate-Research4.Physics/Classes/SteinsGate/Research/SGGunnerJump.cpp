/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */




#include "SGGunnerJump.h"

#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGAnimationDefine.h>
#include <SteinsGate/Research/SGProjectileDefine.h>
#include <SteinsGate/Research/SGActionDefine.h>

SGGunnerJump::SGGunnerJump(SGPlayer* player, SGActionInfo* actionInfo)
	: SGAction(player, actionInfo) {}


// #define GUNNER_ANIMATION_JUMP_START					14
// #define GUNNER_ANIMATION_JUMP_UP					15
// #define GUNNER_ANIMATION_JUMP_DOWN					16
// #define GUNNER_ANIMATION_JUMP_END					17
// 
// #define GUNNER_ANIMATION_JUMP_SHOT_BEGIN			18
// #define GUNNER_ANIMATION_JUMP_SHOT_SHOT				19
// #define GUNNER_ANIMATION_JUMP_SHOT_END				20

void SGGunnerJump::onActionBegin() {

	SGActionManager* pActionManager = m_pPlayer->getActionManager();
	SGAction* pPrevious = pActionManager->getPreviousAction();

	m_bJumpUpbegin = false;
	m_bJumpDownBegin = false;
	m_bCanFire = false;
	m_bFireMode = false;
	m_bWaitForFire = false;
	m_bMoveableX = false;
	m_bMoveableY = false;
	m_bRightFire = false;
	m_iChargedShotCount = 0;
	m_iShotCount = 0;
	m_iMaxShotCount = m_pPlayer->getCharacter()->getBaseInfo()->JumpShotCount[WeaponType::Auto];

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
	} else if (iAnimationCode == GUNNER_ANIMATION_JUMP_SHOT_SHOT) {
		
	}
}

void SGGunnerJump::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {
	int iAnimationCode = animation->getAnimationCode();
	SGCharacter* pCharacter = m_pPlayer->getCharacter();

	if (iAnimationCode == GUNNER_ANIMATION_JUMP_START) {
		m_bJumpUpbegin = true;
		m_bMoveableX = true;
		m_bMoveableY = true;
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

	if (iFrameIndexInAnimation == 1) {
		shot(pCharacter);
	} else if (iFrameIndexInAnimation == 2) {
		m_bWaitForFire = true;
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

	if (m_bWaitForFire) {
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
	if (m_bJumpDownBegin && character->getPositionActorY() == 0) {
		Log("애니메이션 종료");
		character->runAnimation(GUNNER_ANIMATION_JUMP_END);
		m_bJumpDownBegin = false;
	}
}


void SGGunnerJump::shot(SGCharacter* character) {
	if (m_iChargedShotCount <= 0) 
		return;

	if (m_iShotCount >= m_iMaxShotCount)
		return;

	m_iShotCount++;
	--m_iChargedShotCount;

	SGPlayerController* pController = m_pPlayer->getController();
	SpriteDirection_t eDirection = character->getSpriteDirection();
	character->runAnimation(GUNNER_ANIMATION_JUMP_SHOT_SHOT);

	if (eDirection == SpriteDirection::Right) {
		if (pController->isKeyPressed(ControlKey::Left)) {
			m_fMoveSpeedFPSX = 225;
		} else if (pController->isKeyPressed(ControlKey::Right)) {
			m_fMoveSpeedFPSX = 0;
		} else {
			m_fMoveSpeedFPSX = 0.0f;

			character->removeForceX();
			character->addForceX(-50.0f);
		}
	} else if (eDirection == SpriteDirection::Left) {
		if (pController->isKeyPressed(ControlKey::Right)) {
			m_fMoveSpeedFPSX = 225;
		} else if (pController->isKeyPressed(ControlKey::Left)) {
			m_fMoveSpeedFPSX = 0;
		} else {
			m_fMoveSpeedFPSX = 0.0f;
			character->removeForceX();
			character->addForceX(50.0f);
		}
	} else {
		int a = 40;
	}

	float fRemovedYForce = character->removeForceY();

	if (fRemovedYForce > 0.0f) {
		character->addForceY(100.0f);
	}

	

	// 거너 총 종류에따라 프로젝틸 혹은 히트박스 생성
	// 일단 오토건 기준
	WeaponType_t ePlayerWeaponType = WeaponType::Auto;
	FrameEventType_t eFrameEventType = WeaponType::FrameEventType[ePlayerWeaponType];

	int iFrameEventId = -1;

	if (m_bRightFire) {
		switch (ePlayerWeaponType) {
		case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_JUMP_RIGHT; break;
		default: DebugAssertMessage(false, "총 종류가 이상합니다. (1)");
		}
	} else {
		switch (ePlayerWeaponType) {
		case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_JUMP_LEFT; break;
		default: DebugAssertMessage(false, "총 종류가 이상합니다. (2)");
		}
	}

	DebugAssertMessage(iFrameEventId != -1, "프레임 이벤트 ID가 설정되지 않았습니다.");
	runFrameEvent(eFrameEventType, iFrameEventId);
	m_bRightFire = !m_bRightFire;

}