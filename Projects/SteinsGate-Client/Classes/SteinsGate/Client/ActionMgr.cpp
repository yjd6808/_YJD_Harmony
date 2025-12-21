/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:01:05 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ActionMgr.h"

#include <SteinsGate/Client/SGAction.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/AllActions.h>
#include <SteinsGate/Client/Define_Action.h>
#include <SteinsGate/Client/FrameTexture.h>

//////////////////////////////////////////////////////////////////////////////////////////
ActionMgr::ActionMgr(HostPlayer* _pPlayer)
: player_(_pPlayer)
, runningAction_(nullptr)
, previousAction_(nullptr)
, baseAction_{ {} }
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActionMgr::~ActionMgr()
{
	actionMap_.ForEachValueDelete();
	_LogDebug_("액션 매니저 소멸");
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::Init(int _charType)
{
	switch (_charType)
	{
	case CharType::Gunner:
		InitGunnerActions();
		break;
	default:
		DebugAssertMsg(false, "해당 타입의 캐릭터는 존재하지 않습니다.");
	}

	actionMap_.Values()
	          .Extension()
	          .ForEach([this](SGAction* _pAction)
	          {
		          if (_pAction->GetCommand().Count() > 0)
		          {
			          comboTree_.AddComboAction(_pAction);
		          }
	          });
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::InitGunnerActions()
{
	DataManager* pConfig = DataManager::Get();

	baseAction_[BaseAction::Idle] = dbg_new GunnerIdle(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_IDLE));
	baseAction_[BaseAction::Walk] = dbg_new GunnerWalk(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_WALK));
	baseAction_[BaseAction::Run] = dbg_new GunnerRun(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_RUN));
	baseAction_[BaseAction::Attack] = dbg_new  GunnerGunShot(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_GUN_SHOT));
	baseAction_[BaseAction::Sliding] = dbg_new GunnerSliding(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_SLIDING));
	baseAction_[BaseAction::Jump] = dbg_new GunnerJump(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_JUMP));
	baseAction_[BaseAction::Hit] = dbg_new GunnerHit(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_HIT));
	baseAction_[BaseAction::FallDown] = dbg_new GunnerFallDown(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_FALL_DOWN));
	baseAction_[BaseAction::SitRecover] = dbg_new GunnerSitRecover(player_, pConfig->GetActionInfo(DEF_ACTION_GUNNER_SIT_RECOVER));

	actionMap_.Insert(DEF_ACTION_GUNNER_IDLE, baseAction_[BaseAction::Idle]);
	actionMap_.Insert(DEF_ACTION_GUNNER_WALK, baseAction_[BaseAction::Walk]);
	actionMap_.Insert(DEF_ACTION_GUNNER_RUN, baseAction_[BaseAction::Run]);
	actionMap_.Insert(DEF_ACTION_GUNNER_GUN_SHOT, baseAction_[BaseAction::Attack]);
	actionMap_.Insert(DEF_ACTION_GUNNER_SLIDING, baseAction_[BaseAction::Sliding]);
	actionMap_.Insert(DEF_ACTION_GUNNER_JUMP, baseAction_[BaseAction::Jump]);
	actionMap_.Insert(DEF_ACTION_GUNNER_HIT, baseAction_[BaseAction::Hit]);
	actionMap_.Insert(DEF_ACTION_GUNNER_FALL_DOWN, baseAction_[BaseAction::FallDown]);
	actionMap_.Insert(DEF_ACTION_GUNNER_SIT_RECOVER, baseAction_[BaseAction::SitRecover]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::Update(float _dt)
{
	if (runningAction_)
		runningAction_->OnUpdate(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::OnKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	if (runningAction_)
		runningAction_->OnKeyPressed(_pController, _keyCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::OnKeyReleased(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	if (runningAction_)
		runningAction_->OnKeyReleased(_pController, _keyCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::OnKeyPressedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	if (runningAction_)
		runningAction_->OnKeyPressedBefore(_pController, _keyCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::OnKeyReleasedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	if (runningAction_)
		runningAction_->OnKeyReleasedBefore(_pController, _keyCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (runningAction_)
		runningAction_->OnFrameBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (runningAction_)
		runningAction_->OnFrameEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (runningAction_)
		runningAction_->OnAnimationBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (runningAction_)
		runningAction_->OnAnimationEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActionMgr::CanRunAction()
{
	if (runningAction_)
	{
		if (runningAction_->IsForceCancelable())
			return true;

		return false; // 액션을 수행중인데 캔슬 가능한 경우가 아닌 경우
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActionMgr::IsBaseActionRunning(BaseAction_t _baseActionType)
{
	if (baseAction_[_baseActionType] == nullptr)
		return false;

	return baseAction_[_baseActionType] == runningAction_;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActionMgr::IsActionRunning(int _actionCode)
{
	DebugAssertMsg(actionMap_.Exist(_actionCode), "해당 코드값의 액션이 존재하지 않습니다.");

	if (actionMap_[_actionCode] != runningAction_)
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ActionMgr::IsRunnningActionIdleOrWalkAction()
{
	if (runningAction_ == nullptr)
		return true;

	if (runningAction_ == baseAction_[BaseAction::Idle] ||
		runningAction_ == baseAction_[BaseAction::Walk])
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGAction* ActionMgr::GetAction(int _actionCode)
{
	DebugAssertMsg(actionMap_.Exist(_actionCode), "해당 코드값의 액션이 존재하지 않습니다.");
	return actionMap_.Get(_actionCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGAction* ActionMgr::GetBaseAction(BaseAction_t _baseActionType)
{
	DebugAssertMsg(baseAction_[_baseActionType], "해당 기초액션이 아직 구현되지 않았습니다.");
	return baseAction_[_baseActionType];
}

//////////////////////////////////////////////////////////////////////////////////////////
// int Count = 0;
void ActionMgr::RunAction(int _actionCode)
{
	if (!CanRunAction())
		return;

	if (runningAction_)
	{
		runningAction_->Stop();
	}

	runningAction_ = GetAction(_actionCode);
	runningAction_->Init();
	runningAction_->Play();
	// Log("[액션 실행 %05d] %s\n", ++Count, runningAction_->getActionName().Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::RunAction(SGAction* _pAction)
{
	if (!CanRunAction())
		return;

	if (runningAction_)
	{
		runningAction_->Stop();
	}

	runningAction_ = _pAction;
	runningAction_->Init();
	runningAction_->Play();
	// Log("[액션 실행 %05d] %s\n", ++Count, runningAction_->getActionName().Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::RunBaseAction(BaseAction_t _baseActionType)
{
	DebugAssertMsg(baseAction_[_baseActionType], "해당 기초액션이 아직 구현되지 않았습니다.");
	RunAction(baseAction_[_baseActionType]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::StopActionForce()
{
	if (runningAction_ != nullptr)
	{
		runningAction_->OnActionEnd();
	}
	previousAction_ = runningAction_;
	runningAction_ = nullptr;
}
