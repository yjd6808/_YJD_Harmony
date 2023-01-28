/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */


#include "SGGunnerGunShot.h"

#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGAnimationDefine.h>
#include <SteinsGate/Research/SGProjectileDefine.h>

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
	m_bFinalShot = false;
	m_iContinuosCount = 0;
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

void SGGunnerGunShot::onActionEnd() {

}

void SGGunnerGunShot::onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {

	const int iAnimationCode = animation->getAnimationInfo()->Code;


	if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_BEGIN ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_BEGIN ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_LEFT ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_LEFT_DOWN) {
		// 팔뚝 치기
	}
	else if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_SHOT ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT /*||
		iMotionState == MotionState::ShotRightEnd ||
		iMotionState == MotionState::ShotRightDownEnd*/) {
		Log("---- 총 발사! 애니메이션 시작\n");
		m_bNextFireCheck = true;
	}
}

void SGGunnerGunShot::onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* frame) {

	const int iAnimationCode = animation->getAnimationInfo()->Code;
	SGActorSprite* pActorSprite = m_pPlayer->getCharacter()->getActorSprite();

	if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_BEGIN ||
		iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_BEGIN) {
		if (m_bDownShotKeyPressedFirst) {
			pActorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT);
		} else {
			pActorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_SHOT);
		}

		
	}
	else if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_SHOT) {
		if (m_iContinuosCount <= 0) {
			pActorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_END);
			m_bShotEnd = true;
			return;
		}

		shot(pActorSprite, iAnimationCode);
	}
	else if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT) {
		if (m_iContinuosCount <= 0) {
			pActorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_DOWN_END);
			m_bShotEnd = true;
			return;
		}
		shot(pActorSprite, iAnimationCode);
	}
	else if (iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_END ||
			 iAnimationCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_END) {

		if (m_bFinalShot) {
			if (m_bDownShotKeyPressed)
				pActorSprite->runAnimation(GUNNER_ANIMATION_SHOT_LEFT_DOWN);
			else
				pActorSprite->runAnimation(GUNNER_ANIMATION_SHOT_LEFT);
		} else if (m_bShotEnd) {
			stop();
		}
		
	}
	else if (iAnimationCode == GUNNER_ANIMATION_SHOT_LEFT ||
			 iAnimationCode == GUNNER_ANIMATION_SHOT_LEFT_DOWN) {
		stop();
	}
}

void SGGunnerGunShot::onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* frame) {

	int iFrameIndex = frame->getTargetFrameIndex();

	// 거너 총 종류에따라 프로젝틸 혹은 히트박스 생성
	// 일단 오토건 기준

	WeaponType_t ePlayerWeaponType = WeaponType::Auto;
	FrameEventType_t eFrameEventType = WeaponType::FrameEventType[ePlayerWeaponType];

	int iFrameEventId;

	// 우
	if (iFrameIndex == 26) {
		switch (ePlayerWeaponType) {
		case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_RIGHT;
		}

		runFrameEvent(eFrameEventType, iFrameEventId);
	}

	// 좌
	else if (iFrameIndex == 35) {
		switch (ePlayerWeaponType) {
		case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_LEFT;
		}
		runFrameEvent(eFrameEventType, iFrameEventId);
	}

	else if (iFrameIndex == 42) {
		switch (ePlayerWeaponType) {
		case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_RIGHT_DOWN;
		}
		runFrameEvent(eFrameEventType, iFrameEventId);
	}

	else if (iFrameIndex == 51) {
		switch (ePlayerWeaponType) {
		case WeaponType::Auto: iFrameEventId = GUNNER_PROJECTILE_AUTO_LEFT_DOWN;
		}
		runFrameEvent(eFrameEventType, iFrameEventId);
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

void SGGunnerGunShot::onUpdate(float dt) {
	auto controller = m_pPlayer->getController();

	// 방향키 누르고 있는지 여부는 계속 체크
	if (controller->isKeyPressed(ControlKey::Down)) {
		m_bDownShotKeyPressedFirst = true;
		m_bDownShotKeyPressed = true;
		return;
	}

	m_bDownShotKeyPressed = false;
}

void SGGunnerGunShot::shot(SGActorSprite* actorSprite, int animtionCode) {

	// 아직 우측방향으로 쏠 수 있는 경우
	if (m_iRightShotCount > 0) {


		m_iRightShotCount--;
		if (m_bDownShotKeyPressed)
			actorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT);
		else
			actorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_SHOT);
		--m_iContinuosCount;
		return;
	}

	// 우측 방향으로 더이상 못쏘는 경우
	// 반대총으로 쏘도록 한다.
	if (animtionCode == GUNNER_ANIMATION_SHOT_RIGHT_SHOT)
		actorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_END);
	else if (animtionCode == GUNNER_ANIMATION_SHOT_RIGHT_DOWN_SHOT)
		actorSprite->runAnimation(GUNNER_ANIMATION_SHOT_RIGHT_DOWN_END);
	m_bFinalShot = true;
}

