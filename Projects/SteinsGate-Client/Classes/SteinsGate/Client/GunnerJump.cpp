/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GunnerJump.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/Define_Projectile.h>
#include <SteinsGate/Client/Define_Action.h>
#include <SteinsGate/Client/CharInfo.h>

#include "PhysicsComponent.h"

#define MinimumShotHeight 30.0f

GunnerJump::GunnerJump(HostPlayer* player, ActionInfo* actionInfo)
	: GunnerAction(player, actionInfo) {}

// #define DEF_ANIMATION_GUNNER_JUMP_START					14
// #define DEF_ANIMATION_GUNNER_JUMP_UP						15
// #define DEF_ANIMATION_GUNNER_JUMP_DOWN					16
// #define DEF_ANIMATION_GUNNER_JUMP_END					17
// 
// #define DEF_ANIMATION_GUNNER_JUMP_SHOT_BEGIN				18
// #define DEF_ANIMATION_GUNNER_JUMP_SHOT_SHOT				19
// #define DEF_ANIMATION_GUNNER_JUMP_SHOT_END				20

void GunnerJump::onActionBegin() {
	ActionMgr* pActionManager = m_pPlayer->actionManager();
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

	if (pPrevious->getActionCode() == DEF_ACTION_GUNNER_RUN) {
		m_fMoveSpeedFPSX = pPrevious->getMoveSpeedX();
		m_fMoveSpeedFPSY = pPrevious->getMoveSpeedY();
	} else {
		SGAction* pWalkAction = pActionManager->getBaseAction(BaseAction::Walk);
		m_fMoveSpeedFPSX = pWalkAction->getMoveSpeedX();
		m_fMoveSpeedFPSY = pWalkAction->getMoveSpeedY();
	}

	m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_JUMP_START);
}

void GunnerJump::onUpdate(float dt) {
	Character* pCharacter = m_pPlayer;

	updateJumpUp(pCharacter, dt);
	updateJumpDown(pCharacter, dt);
}

void GunnerJump::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* frame) {
	int iAnimationCode = animation->getAnimationCode();
	Character* pCharacter = m_pPlayer;
	PlayerController* pController = m_pPlayer->ctrl();

	// 착지 경지 효과를 위해 움직임 봉인
	if (iAnimationCode == DEF_ANIMATION_GUNNER_JUMP_END) {
		setMoveable(false);
	} 
}

void GunnerJump::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* frame) {
	int iAnimationCode = animation->getAnimationCode();
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (iAnimationCode == DEF_ANIMATION_GUNNER_JUMP_START) {
		setMoveable(true);

		m_bJumpUpbegin = true;
		m_bCanFire = true;

		m_pPlayer->runAnimation(DEF_ANIMATION_GUNNER_JUMP_UP);

		if (pPhysicsComponent)
			pPhysicsComponent->addForceY(m_pPlayer->getBaseInfo()->JumpForce);

	} else if (iAnimationCode == DEF_ANIMATION_GUNNER_JUMP_SHOT_BEGIN) {
		shot(m_pPlayer);
	} else if (iAnimationCode == DEF_ANIMATION_GUNNER_JUMP_END) {
		stop();
	} 
}

void GunnerJump::onFrameBegin(ActorPartAnimation* animation, FrameTexture* frame) {
	
}

void GunnerJump::onFrameEnd(ActorPartAnimation* animation, FrameTexture* frame) {
	int iAnimationCode = animation->getAnimationCode();
	if (iAnimationCode != DEF_ANIMATION_GUNNER_JUMP_SHOT_SHOT) {
		return;
	}

	Character* pCharacter = m_pPlayer;
	int iFrameIndexInAnimation = animation->getFrameIndexInAnimation();

	// jump_shot_shot 73번 인덱스 호출완료시
	if (iFrameIndexInAnimation == 1) {

		// 쏘고 나서 높이가 0인경우 바로 정지시켜주도록 하자.
		if (pCharacter->getPositionActorY() == 0) {
			pCharacter->runAnimation(DEF_ANIMATION_GUNNER_JUMP_END);
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

void GunnerJump::onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode) {
	ControlKey_t controlKey = controller->convertControlKey(keyCode);
	Character* pCharacter = m_pPlayer;

	if (!m_bFireMode)
		controller->updateDirection(controlKey);

	if (controlKey != ControlKey::Attack)
		return;

	if (!m_bCanFire)
		return;

	++m_iChargedShotCount;

	if (!m_bFireMode) {
		pCharacter->runAnimation(DEF_ANIMATION_GUNNER_JUMP_SHOT_BEGIN);
		m_bFireMode = true;
	}
	
	if (m_bWaitForFire && pCharacter->getPositionActorY() >= MinimumShotHeight) {
		shot(pCharacter);
		m_bWaitForFire = false;
	}
}

void GunnerJump::updateJumpUp(Character* character, float dt) {
	if (!m_bJumpUpbegin)
		return;

	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	// Step 1. 상승 중
	if (m_bJumpUpbegin && pPhysicsComponent && pPhysicsComponent->getDownTime() > 0.0f) {
		m_bJumpUpbegin = false;
		m_bJumpDownBegin = true;

		if (!m_bFireMode) {
			character->runAnimation(DEF_ANIMATION_GUNNER_JUMP_DOWN);
		}
	}
}
void GunnerJump::updateJumpDown(Character* character, float dt) {
	if (!m_bJumpDownBegin)
		return;

	// Step 2. 하강 중
	if (m_bJumpDownBegin && character->isOnTheGround()) {
		character->runAnimation(DEF_ANIMATION_GUNNER_JUMP_END);
		m_bJumpDownBegin = false;
	}
}


bool GunnerJump::shot(Character* character) {
	if (m_iChargedShotCount <= 0) 
		return false;

	if (m_iShotCount >= m_iMaxShotCount)
		return false;

	m_iShotCount++;
	--m_iChargedShotCount;
	m_bRightFire = !m_bRightFire;		// 좌/우 토글
	character->runAnimation(DEF_ANIMATION_GUNNER_JUMP_SHOT_SHOT);
	
	reboundX(character);			// X축 반동
	reboundY(character);			// Y축 반동
	createBullet();

	return true;
}


void GunnerJump::reboundX(Character* character) {
	
	SpriteDirection_t eDirection = character->getSpriteDirection();
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	// 반동 초기화
	if (pPhysicsComponent)
		pPhysicsComponent->removeForceX();

	// 1. 우측키를 누른 상태
	switch (eDirection) {
	case SpriteDirection::Right:	reboundXLeft(character);	break;	// 우측으로 바라보고 있으면 좌측 반동
	case SpriteDirection::Left:		reboundXRight(character);	break;	// 좌측으로 바라보고 있으면 우측 반동
	default: DebugAssertMsg(false, "방향이 도대체 어디에요?");
	}
}

void GunnerJump::reboundXLeft(Character* character) {
	PlayerController* pController = m_pPlayer->ctrl();
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

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
		if (pPhysicsComponent)
			pPhysicsComponent->addForceX(m_pBaseInfo->JumpShotForceX[m_eWeaponType] * -1);
		return;
	}

	if (pController->isKeyPressed(ControlKey::Left)) {
		if (pPhysicsComponent)
			pPhysicsComponent->addForceX(m_pBaseInfo->JumpShotForceX[m_eWeaponType] * -1);
		m_fMoveSpeedFPSX = m_pBaseInfo->JumpShotMoveSpeedX[m_eWeaponType];
	}
}

void GunnerJump::reboundXRight(Character* character) {
	PlayerController* pController = m_pPlayer->ctrl();
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	m_bMoveableNegativeX = false;
	m_fMoveSpeedFPSX = 0.0f;

	if (pController->isKeyPressed(ControlKey::Left)) {
		return;
	}

	if (!pController->isMoveKeyPressed()) {
		if (pPhysicsComponent)
			pPhysicsComponent->addForceX(m_pBaseInfo->JumpShotForceX[m_eWeaponType]);
		return;
	}

	if (pController->isKeyPressed(ControlKey::Right)) {
		if (pPhysicsComponent)
			pPhysicsComponent->addForceX(m_pBaseInfo->JumpShotForceX[m_eWeaponType]);

		m_fMoveSpeedFPSX = m_pBaseInfo->JumpShotMoveSpeedX[m_eWeaponType];
	}
}



void GunnerJump::reboundY(Character* character) {
	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (!pPhysicsComponent)
		return;

	// 중력 역행
	float fRemovedYForce = pPhysicsComponent->removeForceY();
	if (fRemovedYForce > 0.0f) {
		pPhysicsComponent->addForceY(m_pBaseInfo->JumpShotForceY[m_eWeaponType]);
	}

}

void GunnerJump::createBullet() {

	// 거너 총 종류에따라 프로젝틸 혹은 히트박스 생성
	FrameEventSpawnType_t eFrameEventType = WeaponType::ShotFrameEventSpawnType[m_eWeaponType];
	int iSpawnCode = InvalidValue_v;

	if (m_bRightFire) {
		switch (m_eWeaponType) {
		case WeaponType::Automatic: iSpawnCode = DEF_PROJECTILE_GUNNER_AUTO_JUMP_RIGHT; break;
		case WeaponType::Revolver: iSpawnCode = DEF_PROJECTILE_GUNNER_AUTO_JUMP_RIGHT; break;
		default: DebugAssertMsg(false, "총 종류가 이상합니다. (1)");
		}
	}
	else {
		switch (m_eWeaponType) {
		case WeaponType::Automatic: iSpawnCode = DEF_PROJECTILE_GUNNER_AUTO_JUMP_LEFT; break;
		case WeaponType::Revolver: iSpawnCode = DEF_PROJECTILE_GUNNER_AUTO_JUMP_LEFT; break;
		default: DebugAssertMsg(false, "총 종류가 이상합니다. (2)");
		}
	}

	DebugAssertMsg(iSpawnCode != InvalidValue_v, "프레임 이벤트 ID가 설정되지 않았습니다.");
	m_pPlayer->runFrameEventSpawn(eFrameEventType, iSpawnCode);
}
