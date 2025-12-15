/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 11:41:05 AM
 * =====================
 *
 * 
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "GunnerGunShot.h"

#include <SteinsGate/Client/HostPlayer.h>
#include <SteinsGate/Client/Define_Animation.h>
#include <SteinsGate/Client/Define_Projectile.h>
#include <SteinsGate/Client/Define_Action.h>
#include <SteinsGate/Client/Define_Effect.h>
#include <SteinsGate/Client/Define_Event.h>
#include <SteinsGate/Client/PhysicsComponent.h>

#define MaxWaitRightShotTime 0.1f // (6fps)

//////////////////////////////////////////////////////////////////////////////////////////
GunnerGunShot::GunnerGunShot(HostPlayer* _pPlayer, ActionInfo* _pActionInfo)
: GunnerAction(_pPlayer, _pActionInfo)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool GunnerGunShot::onConditionCheck()
{
	const int runningActionCode = m_pPlayer->getRunningActionCode();
	return runningActionCode == DEF_ACTION_GUNNER_IDLE || runningActionCode == DEF_ACTION_GUNNER_WALK;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::onActionBegin()
{
	setMoveable(false);

	m_pHitRecorder->setAlreadyHitRecord(true);
	m_pHitRecorder->addSingleHitCallback(
		DEF_EVENT_HIT_GUNNER_GUN_SHOT, CC_CALLBACK_1(GunnerGunShot::onEnemySingleHit, this));
	m_pHitRecorder->addMultiHitCallback(
		DEF_EVENT_HIT_GUNNER_GUN_SHOT, CC_CALLBACK_2(GunnerGunShot::onEnemyMultiHit, this));

	shotEnd_ = false;
	downShotKeyPressedFirst_ = false;
	downShotKeyPressed_ = false;
	nextFireCheck_ = false;
	m_bCancelable = false;

	waitRightShot_ = false;
	waitRightShotTime_ = 0.0f;
	continuosCount_ = 1;
	rightShotCount_ = m_pBaseInfo->shotCount_[m_eWeaponType];

	PlayerController* pController = m_pPlayer->ctrl();
	ActorSprite* pActorSprite = m_pPlayer->getActorSprite();

	if (pController->isKeyPressed(ControlKey::Down))
	{
		downShotKeyPressed_ = true;
		pActorSprite->runAnimation(DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_BEGIN);
	}
	else
	{
		pActorSprite->runAnimation(DEF_ANIMATION_GUNNER_SHOT_RIGHT_BEGIN);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 애니메이트 단위 체크
//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	const int animationCode = _pAnimation->getAnimationInfo()->code_;

	if (animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_SHOT ||
		animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_SHOT)
	{
		nextFireCheck_ = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	const int animationCode = _pAnimation->getAnimationInfo()->code_;
	ActorSprite* pActorSprite = m_pPlayer->getActorSprite();

	if (animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_BEGIN ||
		animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_BEGIN)
	{
		runRightShotAnimation(pActorSprite);
	}
	else if (animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_SHOT ||
		animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_SHOT)
	{
		// 총을 쏠 수 있는 경우 바로 발사 해줌
		if (continuosCount_ > 0)
		{
			// 오른 손을 발사 시도해보고 못쏘면 왼손 발사 시작
			if (!runRightShotAnimation(pActorSprite))
			{
				runLeftShotAnimation(pActorSprite, animationCode);
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
			stop();
		}
	}
	else if (animationCode == DEF_ANIMATION_GUNNER_SHOT_LEFT ||
		animationCode == DEF_ANIMATION_GUNNER_SHOT_LEFT_DOWN)
	{
		stop();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pFrame)
{
	// 왼쪽 공격, 왼쪽 아래 총알 발사 시점을 특정할게 프레임 인덱스 뿐임..
	// 한번만 딱 쏘는 왼손 애니메이션도 3개로 분리하긴 귀찮다.

	const int frameIndex = _pAnimation->getFrameIndex();

	if (frameIndex == 35)
	{
		shotLeft(m_pPlayer->getActorSprite());
	}
	else if (frameIndex == 51)
	{
		shotLeftDown(m_pPlayer->getActorSprite());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::onKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	const ControlKey_t key = _pController->convertControlKey(_keyCode);

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
void GunnerGunShot::onUpdate(float _deltaTime)
{
	PlayerController* pController = m_pPlayer->ctrl();

	updateDownKeyCheck(pController);
	updateRightShotTime(pController, _deltaTime);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::updateDownKeyCheck(PlayerController* _pController)
{
	// 방향키 누르고 있는지 여부는 계속 체크
	if (_pController->isKeyPressed(ControlKey::Down))
	{
		downShotKeyPressedFirst_ = true;
		downShotKeyPressed_ = true;
		return;
	}

	downShotKeyPressed_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::updateRightShotTime(PlayerController* _pController, float _deltaTime)
{
	(void)_pController;

	if (!waitRightShot_)
	{
		return;
	}

	// Step 1. 시간 내로 공격 키를 누른 경우
	ActorSprite* pActorSprite = m_pPlayer->getActorSprite();
	waitRightShotTime_ += _deltaTime;

	if (continuosCount_ > 0)
	{
		runRightShotAnimation(pActorSprite);
		waitRightShot_ = false;
		return;
	}

	if (waitRightShotTime_ < MaxWaitRightShotTime)
	{
		return;
	}

	// Step 2. 시간 내로 공격 키를 안 누른 경우 종료
	stop();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::onEnemySingleHit(HitInfo& _info)
{
	if (m_pHitRecorder->isAlreadyHit(_info.HitTarget))
	{
		return;
	}

	ActorManager::Get()->createEffectOnMapTargetCollision(DEF_EFFECT_KNOCK_SMALL, _info, true);
	_info.HitTarget->hit(_info);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::onEnemyMultiHit(SGHitInfoList& _hitList, int _newHitCount)
{
	(void)_hitList;

	PhysicsComponent* pPhysicsComponent = m_pPlayer->getComponent<PhysicsComponent>();

	if (pPhysicsComponent && _newHitCount > 0)
	{
		pPhysicsComponent->stiffenBody(Const::FPS::_6);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 구현
//////////////////////////////////////////////////////////////////////////////////////////
bool GunnerGunShot::runRightShotAnimation(ActorSprite* _pActorSprite)
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
		shotRight(_pActorSprite);
	}
	else
	{
		shotRightDown(_pActorSprite);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::runLeftShotAnimation(ActorSprite* _pActorSprite, int _animationCode)
{
	// 우측 방향으로 더이상 못쏘는 경우
	// 반대총으로 쏘도록 한다.
	m_pHitRecorder->clearAlreadyHitEnemies();

	if (_animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_SHOT)
	{
		_pActorSprite->runAnimation(DEF_ANIMATION_GUNNER_SHOT_LEFT);
	}
	else if (_animationCode == DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_SHOT)
	{
		_pActorSprite->runAnimation(DEF_ANIMATION_GUNNER_SHOT_LEFT_DOWN);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::shotRight(ActorSprite* _pActorSprite)
{
	_pActorSprite->runAnimation(DEF_ANIMATION_GUNNER_SHOT_RIGHT_SHOT);

	const FrameEventSpawnType_t spawnType = WeaponType::ShotFrameEventSpawnType[m_eWeaponType];
	int spawnCode;

	switch (m_eWeaponType)
	{
	case WeaponType::Automatic: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_RIGHT;
		break;
	case WeaponType::Revolver: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_RIGHT;
		break;
	default: spawnCode = -1;
		break;
	}

	DebugAssertMsg(spawnCode != -1, "프레임 이벤트 ID가 이상합니다. (1)");
	m_pPlayer->runFrameEventSpawn(spawnType, spawnCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::shotRightDown(ActorSprite* _pActorSprite)
{
	_pActorSprite->runAnimation(DEF_ANIMATION_GUNNER_SHOT_RIGHT_DOWN_SHOT);

	const FrameEventSpawnType_t spawnType = WeaponType::ShotFrameEventSpawnType[m_eWeaponType];
	int spawnCode;

	switch (m_eWeaponType)
	{
	case WeaponType::Automatic: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_RIGHT_DOWN;
		break;
	case WeaponType::Revolver: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_RIGHT_DOWN;
		break;
	default: spawnCode = -1;
		break;
	}

	DebugAssertMsg(spawnCode != -1, "프레임 이벤트 ID가 이상합니다. (2)");
	m_pPlayer->runFrameEventSpawn(spawnType, spawnCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::shotLeft(ActorSprite* _pActorSprite)
{
	// 왼쪽 손은 애니메이션 이 함수로 실행 안함.

	const FrameEventSpawnType_t spawnType = WeaponType::ShotFrameEventSpawnType[m_eWeaponType];
	int spawnCode;

	switch (m_eWeaponType)
	{
	case WeaponType::Automatic: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_LEFT;
		break;
	case WeaponType::Revolver: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_LEFT;
		break;
	default: spawnCode = -1;
		break;
	}

	DebugAssertMsg(spawnCode != -1, "프레임 이벤트 ID가 이상합니다. (3)");
	m_pPlayer->runFrameEventSpawn(spawnType, spawnCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GunnerGunShot::shotLeftDown(ActorSprite* _pActorSprite)
{
	// 왼쪽 손은 애니메이션 이 함수로 실행 안함.

	const FrameEventSpawnType_t spawnType = WeaponType::ShotFrameEventSpawnType[m_eWeaponType];
	int spawnCode;

	switch (m_eWeaponType)
	{
	case WeaponType::Automatic: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_LEFT_DOWN;
		break;
	case WeaponType::Revolver: spawnCode = DEF_PROJECTILE_GUNNER_AUTO_LEFT_DOWN;
		break;
	default: spawnCode = -1;
		break;
	}

	DebugAssertMsg(spawnCode != -1, "프레임 이벤트 ID가 이상합니다. (4)");
	m_pPlayer->runFrameEventSpawn(spawnType, spawnCode);
}
