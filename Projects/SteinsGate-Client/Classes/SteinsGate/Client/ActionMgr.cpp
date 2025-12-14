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

////////////////////////////////////////////////////////////////////////////////////////////////////
ActionMgr::ActionMgr(HostPlayer* _pPlayer)
	: player_(_pPlayer)
	, runningAction_(nullptr)
	, previousAction_(nullptr)
	, baseAction_{{}}
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ActionMgr::~ActionMgr()
{
	actionMap_.ForEachValueDelete();
	_LogDebug_("액션 매니저 소멸");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::init(int _charType)
{
	switch (_charType)
	{
	case CharType::Gunner:
		initGunnerActions();
		break;
	default:
		DebugAssertMsg(false, "해당 타입의 캐릭터는 존재하지 않습니다.");
	}

	actionMap_.Values()
		.Extension()
		.ForEach([this](SGAction* action)
		{
			if (action->getCommand().count() > 0)
			{
				comboTree_.addComboAction(action);
			}
		});
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::initGunnerActions()
{
	DataManager* pConfig = DataManager::Get();

	baseAction_[BaseAction::Idle] = dbg_new GunnerIdle(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_IDLE));
	baseAction_[BaseAction::Walk] = dbg_new GunnerWalk(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_WALK));
	baseAction_[BaseAction::Run] = dbg_new GunnerRun(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_RUN));
	baseAction_[BaseAction::Attack] = dbg_new GunnerGunShot(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_GUN_SHOT));
	baseAction_[BaseAction::Sliding] = dbg_new GunnerSliding(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_SLIDING));
	baseAction_[BaseAction::Jump] = dbg_new GunnerJump(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_JUMP));
	baseAction_[BaseAction::Hit] = dbg_new GunnerHit(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_HIT));
	baseAction_[BaseAction::FallDown] = dbg_new GunnerFallDown(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_FALL_DOWN));
	baseAction_[BaseAction::SitRecover] = dbg_new GunnerSitRecover(player_, pConfig->getActionInfo(DEF_ACTION_GUNNER_SIT_RECOVER));

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

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::update(float _dt)
{
	if (runningAction_)
		runningAction_->onUpdate(_dt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::onKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	if (runningAction_)
		runningAction_->onKeyPressed(_pController, _keyCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::onKeyReleased(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	if (runningAction_)
		runningAction_->onKeyReleased(_pController, _keyCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::onKeyPressedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	if (runningAction_)
		runningAction_->onKeyPressedBefore(_pController, _keyCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::onKeyReleasedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode)
{
	if (runningAction_)
		runningAction_->onKeyReleasedBefore(_pController, _keyCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (runningAction_)
		runningAction_->onFrameBegin(_pAnimation, _pTexture);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (runningAction_)
		runningAction_->onFrameEnd(_pAnimation, _pTexture);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (runningAction_)
		runningAction_->onAnimationBegin(_pAnimation, _pTexture);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	if (runningAction_)
		runningAction_->onAnimationEnd(_pAnimation, _pTexture);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ActionMgr::canRunAction()
{
	if (runningAction_)
	{
		if (runningAction_->isForceCancelable())
			return true;

		return false;    // 액션을 수행중인데 캔슬 가능한 경우가 아닌 경우
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ActionMgr::isBaseActionRunning(BaseAction_t _baseActionType)
{
	if (baseAction_[_baseActionType] == nullptr)
		return false;

	return baseAction_[_baseActionType] == runningAction_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ActionMgr::isActionRunning(int _actionCode)
{
	DebugAssertMsg(actionMap_.Exist(_actionCode), "해당 코드값의 액션이 존재하지 않습니다.");

	if (actionMap_[_actionCode] != runningAction_)
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ActionMgr::isRunnningActionIdleOrWalkAction()
{
	if (runningAction_ == nullptr)
		return true;

	if (runningAction_ == baseAction_[BaseAction::Idle] ||
		runningAction_ == baseAction_[BaseAction::Walk])
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
SGAction* ActionMgr::getAction(int _actionCode)
{
	DebugAssertMsg(actionMap_.Exist(_actionCode), "해당 코드값의 액션이 존재하지 않습니다.");
	return actionMap_.Get(_actionCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
SGAction* ActionMgr::getBaseAction(BaseAction_t _baseActionType)
{
	DebugAssertMsg(baseAction_[_baseActionType], "해당 기초액션이 아직 구현되지 않았습니다.");
	return baseAction_[_baseActionType];
}

int Count = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::runAction(int _actionCode)
{
	if (!canRunAction())
		return;

	if (runningAction_)
	{
		runningAction_->stop();
	}

	runningAction_ = getAction(_actionCode);
	runningAction_->init();
	runningAction_->play();
	// Log("[액션 실행 %05d] %s\n", ++Count, runningAction_->getActionName().Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::runAction(SGAction* _pAction)
{
	if (!canRunAction())
		return;

	if (runningAction_)
	{
		runningAction_->stop();
	}

	runningAction_ = _pAction;
	runningAction_->init();
	runningAction_->play();
	// Log("[액션 실행 %05d] %s\n", ++Count, runningAction_->getActionName().Source());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::runBaseAction(BaseAction_t _baseActionType)
{
	DebugAssertMsg(baseAction_[_baseActionType], "해당 기초액션이 아직 구현되지 않았습니다.");
	runAction(baseAction_[_baseActionType]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ActionMgr::stopActionForce()
{
	if (runningAction_ != nullptr)
	{
		runningAction_->onActionEnd();
	}

	previousAction_ = runningAction_;
	runningAction_ = nullptr;
}
