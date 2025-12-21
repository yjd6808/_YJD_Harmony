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

#define MINIMUM_SHOT_HEIGHT 30.0f

GunnerJump::GunnerJump(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

// #define DEF_ANIMATION_GUNNER_JUMP_START					14
// #define DEF_ANIMATION_GUNNER_JUMP_UP						15
// #define DEF_ANIMATION_GUNNER_JUMP_DOWN					16
// #define DEF_ANIMATION_GUNNER_JUMP_END					17
// 
// #define DEF_ANIMATION_GUNNER_JUMP_SHOT_BEGIN				18
// #define DEF_ANIMATION_GUNNER_JUMP_SHOT_SHOT				19
// #define DEF_ANIMATION_GUNNER_JUMP_SHOT_END				20

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::OnActionBegin()
{
	ActionMgr* pActionManager = pPlayer_->GetActionManager();
	SGAction* pPrevious = pActionManager->GetPreviousAction();

	SetMoveable(false);

	isJumpUpBegin_ = false;
	isJumpDownBegin_ = false;
	isFireable_ = false;
	isFireMoveable_ = false;
	isWaitingForFire_ = false;
	isRightFire_ = false;
	chargedShotCount_ = 0;
	shotCount_ = 0;
	maxShotCount_ = pBaseInfo_->jumpShotCount_[weaponType_];

	if (pPrevious->GetActionCode() == DEF_ACTION_GUNNER_RUN)
	{
		moveSpeedFPSX = pPrevious->GetMoveSpeedX();
		moveSpeedFPSY_ = pPrevious->GetMoveSpeedY();
	}
	else
	{
		SGAction* pWalkAction = pActionManager->GetBaseAction(BaseAction::Walk);
		moveSpeedFPSX = pWalkAction->GetMoveSpeedX();
		moveSpeedFPSY_ = pWalkAction->GetMoveSpeedY();
	}

	pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_JUMP_START);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::OnUpdate(float _dt)
{
	Character* pCharacter = pPlayer_;

	UpdateJumpUp(pCharacter, _dt);
	UpdateJumpDown(pCharacter, _dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	int iAnimationCode = _pAnimation->GetAnimationCode();
	Character* pCharacter = pPlayer_;
	PlayerController* pController = pPlayer_->GetController();

	// 착지 경지 효과를 위해 움직임 봉인
	if (iAnimationCode == DEF_ANIMATION_GUNNER_JUMP_END)
	{
		SetMoveable(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	int iAnimationCode = _pAnimation->GetAnimationCode();
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (iAnimationCode == DEF_ANIMATION_GUNNER_JUMP_START)
	{
		SetMoveable(true);

		isJumpUpBegin_ = true;
		isFireable_ = true;

		pPlayer_->RunAnimation(DEF_ANIMATION_GUNNER_JUMP_UP);

		if (pPhysicsComponent)
			pPhysicsComponent->AddForceY(pPlayer_->GetBaseInfo()->jumpForce_);
	}
	else if (iAnimationCode == DEF_ANIMATION_GUNNER_JUMP_SHOT_BEGIN)
	{
		Shot(pPlayer_);
	}
	else if (iAnimationCode == DEF_ANIMATION_GUNNER_JUMP_END)
	{
		Stop();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	int iAnimationCode = _pAnimation->GetAnimationCode();
	if (iAnimationCode != DEF_ANIMATION_GUNNER_JUMP_SHOT_SHOT)
	{
		return;
	}

	Character* pCharacter = pPlayer_;
	int iFrameIndexInAnimation = _pAnimation->GetFrameIndexInAnimation();

	// jump_shot_shot 73번 인덱스 호출완료시
	if (iFrameIndexInAnimation == 1)
	{
		// 쏘고 나서 높이가 0인경우 바로 정지시켜주도록 하자.
		if (pCharacter->GetPositionActorY() == 0)
		{
			pCharacter->RunAnimation(DEF_ANIMATION_GUNNER_JUMP_END);
			isJumpDownBegin_ = false;
			return;
		}

		// 높이가 그래도 어느정도 있어야 쏠 수 잇도록 하자.
		if (pCharacter->GetPositionActorY() >= MINIMUM_SHOT_HEIGHT && !Shot(pCharacter))
		{
			// 시간내로 총을 못쏜 경우에대한 처리, 공중에선 쏠 수 있는 횟수만 여유롭다면 언제든지 마음대로 계속 총을 쏠 수 있다.
			isWaitingForFire_ = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::OnKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	ControlKey_t controlKey = _pController->ConvertControlKey(_keyCode);
	Character* pCharacter = pPlayer_;

	if (!isFireMoveable_)
		_pController->UpdateDirection(controlKey);

	if (controlKey != ControlKey::Attack)
		return;

	if (!isFireable_)
		return;

	++chargedShotCount_;

	if (!isFireMoveable_)
	{
		pCharacter->RunAnimation(DEF_ANIMATION_GUNNER_JUMP_SHOT_BEGIN);
		isFireMoveable_ = true;
	}

	if (isWaitingForFire_ && pCharacter->GetPositionActorY() >= MINIMUM_SHOT_HEIGHT)
	{
		Shot(pCharacter);
		isWaitingForFire_ = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::UpdateJumpUp(Character* _pChar, float _dt)
{
	if (!isJumpUpBegin_)
		return;

	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	// Step 1. 상승 중
	if (isJumpUpBegin_ && pPhysicsComponent && pPhysicsComponent->GetDownTime() > 0.0f)
	{
		isJumpUpBegin_ = false;
		isJumpDownBegin_ = true;

		if (!isFireMoveable_)
		{
			_pChar->RunAnimation(DEF_ANIMATION_GUNNER_JUMP_DOWN);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::UpdateJumpDown(Character* _pChar, float _dt)
{
	if (!isJumpDownBegin_)
		return;

	// Step 2. 하강 중
	if (isJumpDownBegin_ && _pChar->IsOnTheGround())
	{
		_pChar->RunAnimation(DEF_ANIMATION_GUNNER_JUMP_END);
		isJumpDownBegin_ = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GunnerJump::Shot(Character* _pChar)
{
	if (chargedShotCount_ <= 0)
		return false;

	if (shotCount_ >= maxShotCount_)
		return false;

	shotCount_++;
	--chargedShotCount_;
	isRightFire_ = !isRightFire_; // 좌/우 토글
	_pChar->RunAnimation(DEF_ANIMATION_GUNNER_JUMP_SHOT_SHOT);

	ReboundX(_pChar); // X축 반동
	ReboundY(_pChar); // Y축 반동
	CreateBullet();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::ReboundX(Character* _pChar)
{
	SpriteDirection_t eDirection = _pChar->GetSpriteDirection();
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	// 반동 초기화
	if (pPhysicsComponent)
		pPhysicsComponent->RemoveForceX();

	// 1. 우측키를 누른 상태
	switch (eDirection)
	{
	case SpriteDirection::Right: ReboundXLeft(_pChar);
		break; // 우측으로 바라보고 있으면 좌측 반동
	case SpriteDirection::Left: ReboundXRight(_pChar);
		break; // 좌측으로 바라보고 있으면 우측 반동
	default: DebugAssertMsg(false, "방향이 도대체 어디에요?");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::ReboundXLeft(Character* _pChar)
{
	PlayerController* pController = pPlayer_->GetController();
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	/*
	 * 생각 정리.
	 * 1. 우측으로 총을 쏠때 우측키를 누른 상태면 아무 것도 없음
	 * 2. 우측으로 총을 쏠때 아무키를 안누른 상태면 X 반동만 있어야함
	 * 3. 우측으로 총을 쏠때 왼쪽 키를 누른 상태면 X 반동 + 이동속도 추가
	 * 코드로 옮기자.
	 */

	// 우측으로 한번이라도 쏜 이후부터는 우측 방향 이동 불가능
	isMoveablePositiveX_ = false;
	moveSpeedFPSX = 0.0f;

	if (pController->IsKeyPressed(ControlKey::Right))
	{
		return;
	}

	if (!pController->IsMoveKeyPressed())
	{
		if (pPhysicsComponent)
			pPhysicsComponent->AddForceX(pBaseInfo_->jumpShotForceX_[weaponType_] * -1);
		return;
	}

	if (pController->IsKeyPressed(ControlKey::Left))
	{
		if (pPhysicsComponent)
			pPhysicsComponent->AddForceX(pBaseInfo_->jumpShotForceX_[weaponType_] * -1);
		moveSpeedFPSX = pBaseInfo_->jumpShotMoveSpeedX_[weaponType_];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::ReboundXRight(Character* _pChar)
{
	PlayerController* pController = pPlayer_->GetController();
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	isMoveableNegativeX_ = false;
	moveSpeedFPSX = 0.0f;

	if (pController->IsKeyPressed(ControlKey::Left))
	{
		return;
	}

	if (!pController->IsMoveKeyPressed())
	{
		if (pPhysicsComponent)
			pPhysicsComponent->AddForceX(pBaseInfo_->jumpShotForceX_[weaponType_]);
		return;
	}

	if (pController->IsKeyPressed(ControlKey::Right))
	{
		if (pPhysicsComponent)
			pPhysicsComponent->AddForceX(pBaseInfo_->jumpShotForceX_[weaponType_]);

		moveSpeedFPSX = pBaseInfo_->jumpShotMoveSpeedX_[weaponType_];
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::ReboundY(Character* _pChar)
{
	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (!pPhysicsComponent)
		return;

	// 중력 역행
	float fRemovedYForce = pPhysicsComponent->RemoveForceY();
	if (fRemovedYForce > 0.0f)
	{
		pPhysicsComponent->AddForceY(pBaseInfo_->jumpShotForceY_[weaponType_]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerJump::CreateBullet()
{
	// 거너 총 종류에따라 프로젝틸 혹은 히트박스 생성
	FrameEventSpawnType_t eFrameEventType = WeaponType::ShotFrameEventSpawnType[weaponType_];
	int iSpawnCode = InvalidValue_v;

	if (isRightFire_)
	{
		switch (weaponType_)
		{
		case WeaponType::Automatic: iSpawnCode = DEF_PROJECTILE_GUNNER_AUTO_JUMP_RIGHT;
			break;
		case WeaponType::Revolver: iSpawnCode = DEF_PROJECTILE_GUNNER_AUTO_JUMP_RIGHT;
			break;
		default: DebugAssertMsg(false, "총 종류가 이상합니다. (1)");
		}
	}
	else
	{
		switch (weaponType_)
		{
		case WeaponType::Automatic: iSpawnCode = DEF_PROJECTILE_GUNNER_AUTO_JUMP_LEFT;
			break;
		case WeaponType::Revolver: iSpawnCode = DEF_PROJECTILE_GUNNER_AUTO_JUMP_LEFT;
			break;
		default: DebugAssertMsg(false, "총 종류가 이상합니다. (2)");
		}
	}

	DebugAssertMsg(iSpawnCode != InvalidValue_v, "프레임 이벤트 ID가 설정되지 않았습니다.");
	pPlayer_->RunFrameEventSpawn(eFrameEventType, iSpawnCode);
}
