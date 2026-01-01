/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 * 
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "GunnerGunShot.h"

#include <sgcl/HostPlayer.h>
#include <sgcl/Define_Animation.h>
#include <sgcl/Define_Projectile.h>
#include <sgcl/Define_Action.h>
#include <sgcl/Define_Effect.h>
#include <sgcl/Define_Event.h>
#include <sgcl/PhysicsComponent.h>

#define MaxWaitRightShotTime 0.1f // (6fps)

//////////////////////////////////////////////////////////////////////////////////////////
GunnerGunShot::GunnerGunShot(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GunnerGunShot::OnConditionCheck()
{
	const int runningActionCode = pPlayer_->GetRunningActionCode();
	return runningActionCode == DEF_ACTION_GUNNER_IDLE || runningActionCode == DEF_ACTION_GUNNER_WALK;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::OnActionBegin()
{
	SetMoveable(false);

	pHitRecorder_->SetAlreadyHitRecord(true);
	pHitRecorder_->AddSingleHitCallback(
		DEF_EVENT_HIT_GUNNER_GUN_SHOT, CC_CALLBACK_1(GunnerGunShot::OnEnemySingleHit, this));
	pHitRecorder_->AddMultiHitCallback(
		DEF_EVENT_HIT_GUNNER_GUN_SHOT, CC_CALLBACK_2(GunnerGunShot::OnEnemyMultiHit, this));

	shotEnd_ = false;
	downShotKeyPressedFirst_ = false;
	downShotKeyPressed_ = false;
	nextFireCheck_ = false;
	isCancelable_ = false;

	waitRightShot_ = false;
	waitRightShotTime_ = 0.0f;
	continuosCount_ = 1;
	rightShotCount_ = pBaseInfo_->shotCount_[weaponType_];

	PlayerController* pController = pPlayer_->GetController();
	ActorSprite* pActorSprite = pPlayer_->GetActorSprite();

	if (pController->IsKeyPressed(ControlKey::Down))
	{
		downShotKeyPressed_ = true;
		pActorSprite->RunAnimation(DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_BEGIN);
	}
	else
	{
		pActorSprite->RunAnimation(DEF_ANIMATION_GUNNER_SHOT_RIGHT_BEGIN);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 애니메이트 단위 체크
//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	const int animationCode = _pAnimation->GetAnimationInfo()->code_;

	if (animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_SHOT ||
		animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_SHOT)
	{
		nextFireCheck_ = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	const int animationCode = _pAnimation->GetAnimationInfo()->code_;
	ActorSprite* pActorSprite = pPlayer_->GetActorSprite();

	if (animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_BEGIN ||
		animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_BEGIN)
	{
		RunRightShotAnimation(pActorSprite);
	}
	else if (animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_SHOT ||
		animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_SHOT)
	{
		// 총을 쏠 수 있는 경우 바로 발사 해줌
		if (continuosCount_ > 0)
		{
			// 오른 손을 발사 시도해보고 못쏘면 왼손 발사 시작
			if (!RunRightShotAnimation(pActorSprite))
			{
				RunLeftShotAnimation(pActorSprite, animationCode);
			}
		}
		// 아직 오른쪽 총을 쏠 수 있지만 키 입력을 더 안한 경우
		else if (rightShotCount_ > 0)
		{
			waitRightShotTime_ = 0.0f;
			waitRightShot_ = true;
		}
		else
		{
			// 오른손으로 더 쏠 수도 없고 키 입력도 없는 경우 정지
			Stop();
		}
	}
	else if (animationCode == DEF_ANIMATION_GUNNER_SHOT_LEFT ||
		animationCode == DEF_ANIMATION_GUNNER_SHOT_LEFT_DOWN)
	{
		Stop();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	// 왼쪽 공격, 왼쪽 아래 총알 발사 시점을 특정할게 프레임 인덱스 뿐임..
	// 한번만 딱 쏘는 왼손 애니메이션도 3개로 분리하긴 귀찮다.

	const int frameIndex = _pAnimation->GetFrameIndex();

	if (frameIndex == 35)
	{
		ShotLeft(pPlayer_->GetActorSprite());
	}
	else if (frameIndex == 51)
	{
		ShotLeftDown(pPlayer_->GetActorSprite());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::OnKeyPressed(PlayerController* _pController, c2d::EventKeyboard::KeyCode _keyCode)
{
	const ControlKey_t key = _pController->ConvertControlKey(_keyCode);

	if (!nextFireCheck_)
	{
		return;
	}

	if (key == ControlKey::Attack)
	{
		++continuosCount_;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 업데이트 기능
//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::OnUpdate(float _deltaTime)
{
	PlayerController* pController = pPlayer_->GetController();

	UpdateDownKeyCheck(pController);
	UpdateRightShotTime(pController, _deltaTime);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::UpdateDownKeyCheck(PlayerController* _pController)
{
	// 방향키 누르고 있는지 여부는 계속 체크
	if (_pController->IsKeyPressed(ControlKey::Down))
	{
		downShotKeyPressedFirst_ = true;
		downShotKeyPressed_ = true;
		return;
	}

	downShotKeyPressed_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::UpdateRightShotTime(PlayerController* _pController, float _deltaTime)
{
	(void)_pController;

	if (!waitRightShot_)
	{
		return;
	}

	// Step 1. 시간 내로 공격 키를 누른 경우
	ActorSprite* pActorSprite = pPlayer_->GetActorSprite();
	waitRightShotTime_ += _deltaTime;

	if (continuosCount_ > 0)
	{
		RunRightShotAnimation(pActorSprite);
		waitRightShot_ = false;
		return;
	}

	if (waitRightShotTime_ < MaxWaitRightShotTime)
	{
		return;
	}

	// Step 2. 시간 내로 공격 키를 안 누른 경우 종료
	Stop();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::OnEnemySingleHit(HitInfo& _info)
{
	if (pHitRecorder_->IsAlreadyHit(_info.pHitTarget_))
	{
		return;
	}

	ActorManager::Get()->CreateEffectOnMapTargetCollision(DEF_EFFECT_KNOCK_SMALL, _info, true);
	_info.pHitTarget_->Hit(_info);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::OnEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount)
{
	(void)_hitList;

	PhysicsComponent* pPhysicsComponent = pPlayer_->GetComponent<PhysicsComponent>();

	if (pPhysicsComponent && _newHitCount > 0)
	{
		pPhysicsComponent->StiffenBody(Const::FPS::_6);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 구현
//////////////////////////////////////////////////////////////////////////////////////////
bool GunnerGunShot::RunRightShotAnimation(ActorSprite* _pActorSprite)
{
	if (rightShotCount_ <= 0)
	{
		return false;
	}

	// 아직 우측방향으로 쏠 수 있는 경우
	--rightShotCount_;
	--continuosCount_;

	if (!downShotKeyPressed_)
	{
		ShotRight(_pActorSprite);
	}
	else
	{
		ShotRightDown(_pActorSprite);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::RunLeftShotAnimation(ActorSprite* _pActorSprite, int _animationCode)
{
	// 우측 방향으로 더이상 못쏘는 경우
	// 반대총으로 쏘도록 한다.
	pHitRecorder_->ClearAlreadyHitEnemies();

	if (_animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_SHOT)
	{
		_pActorSprite->RunAnimation(DEF_ANIMATION_GUNNER_SHOT_LEFT);
	}
	else if (_animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_SHOT)
	{
		_pActorSprite->RunAnimation(DEF_ANIMATION_GUNNER_SHOT_LEFT_DOWN);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::ShotRight(ActorSprite* _pActorSprite)
{
	_pActorSprite->RunAnimation(DEF_ANIMATION_GUNNER_SHOT_RIGHT_SHOT);

	const FrameEventSpawnType_t spawnType = WeaponType::ShotFrameEventSpawnType[weaponType_];
	int spawnCode;

	switch (weaponType_)
	{
	case WeaponType::Automatic: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_RIGHT;
		break;
	case WeaponType::Revolver: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_RIGHT;
		break;
	default: spawnCode = -1;
		break;
	}

	jc_assert_msg(spawnCode != -1, "프레임 이벤트 ID가 이상합니다. (1)");
	pPlayer_->RunFrameEventSpawn(spawnType, spawnCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::ShotRightDown(ActorSprite* _pActorSprite)
{
	_pActorSprite->RunAnimation(DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_SHOT);

	const FrameEventSpawnType_t spawnType = WeaponType::ShotFrameEventSpawnType[weaponType_];
	int spawnCode;

	switch (weaponType_)
	{
	case WeaponType::Automatic: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_RIGHT_DOWN;
		break;
	case WeaponType::Revolver: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_RIGHT_DOWN;
		break;
	default: spawnCode = -1;
		break;
	}

	jc_assert_msg(spawnCode != -1, "프레임 이벤트 ID가 이상합니다. (2)");
	pPlayer_->RunFrameEventSpawn(spawnType, spawnCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::ShotLeft(ActorSprite* _pActorSprite)
{
	// 왼쪽 손은 애니메이션 이 함수로 실행 안함.

	const FrameEventSpawnType_t spawnType = WeaponType::ShotFrameEventSpawnType[weaponType_];
	int spawnCode;

	switch (weaponType_)
	{
	case WeaponType::Automatic: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_LEFT;
		break;
	case WeaponType::Revolver: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_LEFT;
		break;
	default: spawnCode = -1;
		break;
	}

	jc_assert_msg(spawnCode != -1, "프레임 이벤트 ID가 이상합니다. (3)");
	pPlayer_->RunFrameEventSpawn(spawnType, spawnCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::ShotLeftDown(ActorSprite* _pActorSprite)
{
	// 왼쪽 손은 애니메이션 이 함수로 실행 안함.

	const FrameEventSpawnType_t spawnType = WeaponType::ShotFrameEventSpawnType[weaponType_];
	int spawnCode;

	switch (weaponType_)
	{
	case WeaponType::Automatic: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_LEFT_DOWN;
		break;
	case WeaponType::Revolver: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_LEFT_DOWN;
		break;
	default: spawnCode = -1;
		break;
	}

	jc_assert_msg(spawnCode != -1, "프레임 이벤트 ID가 이상합니다. (4)");
	pPlayer_->RunFrameEventSpawn(spawnType, spawnCode);
}
