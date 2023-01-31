/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 * 
 */


#include "SGGunnerGunShot.h"

#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGAnimationDefine.h>
#include <SteinsGate/Research/SGProjectileDefine.h>

#define MaxWaitRightShotTime 0.1f // (6fps)

// #define GUNNER_ANIMATION_SHOT_RIGHT_BEGIN			6
// #define GUNNER_ANIMATION_SHOT_RIGHT_SHOT				7
// #define GUNNER_ANIMATION_SHOT_RIGHT_DOWN_BEGIN		9
// #define GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT		10

// #define GUNNER_ANIMATION_SHOT_LEFT					12
// #define GUNNER_ANIMATION_SHOT_LEFT_DOWN				13

SGGunnerGunShot::SGGunnerGunShot(SGPlayer* player, SGActionInfo* actionInfo)
	: SGGunnerAction(player, actionInfo) {}

void SGGunnerGunShot::onActionBegin() {
	SGGunnerAction::onActionBegin();

	setMoveable(false);

	m_bShotEnd = false;
	m_bDownShotKeyPressedFirst = false;
	m_bDownShotKeyPressed = false;
	m_bNextFireCheck = false;
	m_bCancelable = false;

	m_bWaitRightShot = false;
	m_fWaitRightShotTime = 0.0f;
	m_iContinuosCount = 1;
	m_iRightShotCount = m_pBaseInfo->ShotCount[m_eWeaponType];


	SGPlayerController* pController = m_pPlayer->getController();
	SGActorSprite* pActorSprite = m_pPlayer->getCharacter()->getActorSprite();

	if (pController->isKeyPressed(ControlKey::Down)) {
		m_bDownShotKeyPressed = true;
		pActorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_DOWN_BEGIN);
	} else {
		pActorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_BEGIN);
	}
}

// ================================================================
// 애니메이트 단위 체크
// ================================================================

void SGGunnerGunShot::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {

	const int iAnimationCode = animation->getAnimationInfo()->Code;


	if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_BEGIN ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_BEGIN ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_LEFT ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_LEFT_DOWN) {
		// 팔뚝 치기
	}
	else if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_SHOT ||
			 iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT) {
		m_bNextFireCheck = true;
	} 
}

void SGGunnerGunShot::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {

	const int iAnimationCode = animation->getAnimationInfo()->Code;
	SGActorSprite* pActorSprite = m_pPlayer->getActorSprite();

	if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_BEGIN ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_BEGIN) {
		runRightShotAnimation(pActorSprite);
	} else if (	iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_SHOT || 
				iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT) {


		// 총을 쏠 수 있는 경우 바로 발사 해줌
		if (m_iContinuosCount > 0) {

			// 오른 손을 발사 시도해보고 못쏘면 왼손 발사 시작
			if (!runRightShotAnimation(pActorSprite)) {
				runLeftShotAnimation(pActorSprite, iAnimationCode);
			}
		}

		// 아직 오른쪽 총을 쏠 수 있지만 키 입력을 더 안한 경우
		else if (m_iRightShotCount > 0) {

			m_fWaitRightShotTime = 0.0f;
			m_bWaitRightShot = true;
		} else {
			// 오른손으로 더 쏠 수도 없고 키 입력도 없는 경우 정지
			stop();
		}
	} else if (
		iAnimationCode == GUNNER_ANIMATION_SHOT_LEFT ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_LEFT_DOWN) {
		stop();
	}
}

void SGGunnerGunShot::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {

	// 왼쪽 공격, 왼쪽 아래 총알 발사 시점을 특정할게 프레임 인덱스 뿐임..
	// 한번만 딱 쏘는 왼손 애니메이션도 3개로 분리하긴 귀찮다.
	int iFrameIndex = frame->getFrameIndex();
	if (iFrameIndex == 35) {
		shotLeft(m_pPlayer->getActorSprite());
	} else if (iFrameIndex == 51) {
		shotLeftDown(m_pPlayer->getActorSprite());
	}
}

void SGGunnerGunShot::onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	ControlKey_t key = controller->convertControlKey(keyCode);
	if (!m_bNextFireCheck)
		return;

	if (key == ControlKey::Attack) {
		++m_iContinuosCount;
	}
}


// ================================================================
// 업데이트 기능
// ================================================================

void SGGunnerGunShot::onUpdate(float dt) {
	SGPlayerController* controller = m_pPlayer->getController();

	updateDownKeyCheck(controller);
	updateRightShotTime(controller, dt);
}

void SGGunnerGunShot::updateDownKeyCheck(SGPlayerController* controller) {
	// 방향키 누르고 있는지 여부는 계속 체크
	if (controller->isKeyPressed(ControlKey::Down)) {
		m_bDownShotKeyPressedFirst = true;
		m_bDownShotKeyPressed = true;
		return;
	}

	m_bDownShotKeyPressed = false;
}

void SGGunnerGunShot::updateRightShotTime(SGPlayerController* controller, float dt) {
	if (!m_bWaitRightShot) {
		return;
	}

	// Step 1. 시간 내로 공격 키를 누른 경우
	SGActorSprite* pActorSprite = m_pPlayer->getActorSprite();
	m_fWaitRightShotTime += dt;

	if (m_iContinuosCount > 0) {	// 공격키를 입력한 경우
		runRightShotAnimation(pActorSprite);
		m_bWaitRightShot = false;
		return;
	}

	if (m_fWaitRightShotTime < MaxWaitRightShotTime) {
		return;
	}

	// Step 2. 시간 내로 공격 키를 안 누른 경우 종료
	stop();
}


// ================================================================
// 구현
// ================================================================



bool SGGunnerGunShot::runRightShotAnimation(SGActorSprite* actorSprite) {

	if (m_iRightShotCount <= 0)
		return false;

	// 아직 우측방향으로 쏠 수 있는 경우
	m_iRightShotCount--;
	--m_iContinuosCount;

	if (!m_bDownShotKeyPressed) {
		shotRight(actorSprite);
	} else {
		shotRightDown(actorSprite);
	}

	return true;
}

void SGGunnerGunShot::runLeftShotAnimation(SGActorSprite* actorSprite, int animationCode) {
	// 우측 방향으로 더이상 못쏘는 경우
	// 반대총으로 쏘도록 한다.

	if (animationCode == GUNNER_ANIMATION_SHOT_RIGHT_SHOT)
		actorSprite->runAnimation(GUNNER_ANIMATION_SHOT_LEFT);
	else if (animationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT)
		actorSprite->runAnimation(GUNNER_ANIMATION_SHOT_LEFT_DOWN);
}


void SGGunnerGunShot::shotRight(SGActorSprite* actorSprite) {
	actorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_SHOT);	

	FrameEventType_t frameEventType = WeaponType::ShotFrameEventType[m_eWeaponType];
	int iFrameEventId;

	switch (m_eWeaponType) {
	case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_RIGHT; break;
	default: iFrameEventId = -1;
	}

	DebugAssertMessage(iFrameEventId != -1, "프레임 이벤트 ID가 이상합니다. (1)");
	runFrameEvent(frameEventType, iFrameEventId);
}

void SGGunnerGunShot::shotRightDown(SGActorSprite* actorSprite) {

	actorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT);
	FrameEventType_t frameEventType = WeaponType::ShotFrameEventType[m_eWeaponType];
	int iFrameEventId;

	switch (m_eWeaponType) {
	case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_RIGHT_DOWN; break;
	default: iFrameEventId = -1;
	}

	DebugAssertMessage(iFrameEventId != -1, "프레임 이벤트 ID가 이상합니다. (2)");
	runFrameEvent(frameEventType, iFrameEventId);
	
}

void SGGunnerGunShot::shotLeft(SGActorSprite* actorSprite) {
	// 왼쪽 손은 애니메이션 이 함수로 실행 안함.

	FrameEventType_t frameEventType = WeaponType::ShotFrameEventType[m_eWeaponType]; 
	int iFrameEventId;

	switch (m_eWeaponType) {
	case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_LEFT; break;
	default: iFrameEventId = -1;
	}

	DebugAssertMessage(iFrameEventId != -1, "프레임 이벤트 ID가 이상합니다. (3)");
	runFrameEvent(frameEventType, iFrameEventId);
}

void SGGunnerGunShot::shotLeftDown(SGActorSprite* actorSprite) {
	// 왼쪽 손은 애니메이션 이 함수로 실행 안함.

	FrameEventType_t frameEventType = WeaponType::ShotFrameEventType[m_eWeaponType];
	int iFrameEventId;

	switch (m_eWeaponType) {
	case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_LEFT_DOWN; break;
	default: iFrameEventId = -1;
	}

	DebugAssertMessage(iFrameEventId != -1, "프레임 이벤트 ID가 이상합니다. (4)");
	runFrameEvent(frameEventType, iFrameEventId);
}

