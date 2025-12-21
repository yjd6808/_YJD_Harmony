/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "HostPlayer.h"

#include <SteinsGate/Client/CharInfo.h>
#include <SteinsGate/Client/MapLayer.h>
#include <SteinsGate/Client/Define_Action.h>
#include <SteinsGate/Client/AttackDataInfo.h>
#include <SteinsGate/Client/CharacterListener.h>
#include <SteinsGate/Client/Inven.h>

//////////////////////////////////////////////////////////////////////////////////////////
HostPlayer::HostPlayer()
: pActionManager_(nullptr)
, pController_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
HostPlayer::~HostPlayer()
{
	// 삭제
	CC_SAFE_DELETE(pController_);
	CC_SAFE_DELETE(pActionManager_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::Initialize()
{
	playerData_.CharType = CharType::Gunner;
	m_VisualInfo = Core::Contents.Inven->GetVisualInfo(playerData_.CharType);
	m_pBaseInfo = Core::DataManager->GetCharInfo(playerData_.CharType);

	Player::Initialize();
	InitActionManager();
	InitController();

	pActionManager_->RunBaseAction(BaseAction::Idle);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::InitActionManager()
{
	CC_SAFE_DELETE(pActionManager_);

	pActionManager_ = dbg_new ActionMgr(this);
	pActionManager_->Init(m_pBaseInfo->code_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::InitController()
{
	CC_SAFE_DELETE(pController_);
	DebugAssertMsg(pActionManager_, "이 함수를 호출전에 액션 매니저 세팅을 먼저 해주세요.");
	pController_ = dbg_new PlayerController(this, pActionManager_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::InitListeners()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::Hit(const HitInfo& _hitInfo)
{
	Player::Hit(_hitInfo);

	if (_hitInfo.pAttackDataInfo_->isFallDownAttack_)
	{
		PlayBaseActionForce(BaseAction::FallDown);
		return;
	}

	PlayBaseActionForce(BaseAction::Hit);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::RemoveActionManager()
{
	JCORE_DELETE_SAFE(pActionManager_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::RemoveController()
{
	JCORE_DELETE_SAFE(pController_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::update(float _dt)
{
	Player::update(_dt);

	if (pController_)
		pController_->Update(_dt);

	if (pActionManager_)
		pActionManager_->Update(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::OnKeyPressed(SGEventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent)
{
	if (pController_)
		pController_->OnKeyPressed(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::OnKeyReleased(SGEventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent)
{
	if (pController_)
		pController_->OnKeyReleased(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	Player::OnFrameBegin(_pAnimation, _pTexture);

	if (pActionManager_)
		pActionManager_->OnFrameBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	Player::OnFrameEnd(_pAnimation, _pTexture);

	if (pActionManager_)
		pActionManager_->OnFrameEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	Player::OnAnimationBegin(_pAnimation, _pTexture);

	if (pActionManager_)
		pActionManager_->OnAnimationBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	Player::OnAnimationEnd(_pAnimation, _pTexture);

	if (pActionManager_)
		pActionManager_->OnAnimationEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::PlayAction(int _actionCode)
{
	pActionManager_->RunAction(_actionCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::PlayActionForce(int _actionCode)
{
	pActionManager_->StopActionForce();
	pActionManager_->RunAction(_actionCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::PlayBaseActionForce(BaseAction_t _baseActionType)
{
	pActionManager_->StopActionForce();
	pActionManager_->RunBaseAction(_baseActionType);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::PlayBaseAction(BaseAction_t _baseAction)
{
	pActionManager_->RunBaseAction(_baseAction);
}

//////////////////////////////////////////////////////////////////////////////////////////
int HostPlayer::GetRunningActionCode()
{
	SGAction* pAction = pActionManager_->GetRunningAction();
	if (pAction == nullptr)
		return InvalidValue_v;
	return pAction->GetActionCode();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActionMgr* HostPlayer::GetActionManager()
{
	DebugAssertMsg(pActionManager_, "액션 매니저가 세팅되지 않았습니다.");
	return pActionManager_;
}

//////////////////////////////////////////////////////////////////////////////////////////
PlayerController* HostPlayer::GetController()
{
	DebugAssertMsg(pController_, "플레이어 컨트롤러가 세팅되지 않았습니다.");
	return pController_;
}

//////////////////////////////////////////////////////////////////////////////////////////
