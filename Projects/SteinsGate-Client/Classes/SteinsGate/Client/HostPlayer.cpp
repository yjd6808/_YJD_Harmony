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
: actionManager_(nullptr)
, controller_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
HostPlayer::~HostPlayer()
{
	// 삭제
	CC_SAFE_DELETE(controller_);
	CC_SAFE_DELETE(actionManager_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::initialize()
{
	playerData_.CharType = CharType::Gunner;
	m_VisualInfo = Core::Contents.Inven->getVisualInfo(playerData_.CharType);
	m_pBaseInfo = Core::DataManager->getCharInfo(playerData_.CharType);

	Player::initialize();
	initActionManager();
	initController();

	actionManager_->runBaseAction(BaseAction::Idle);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::initActionManager()
{
	CC_SAFE_DELETE(actionManager_);

	actionManager_ = dbg_new ActionMgr(this);
	actionManager_->init(m_pBaseInfo->code_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::initController()
{
	CC_SAFE_DELETE(controller_);
	DebugAssertMsg(actionManager_, "이 함수를 호출전에 액션 매니저 세팅을 먼저 해주세요.");
	controller_ = dbg_new PlayerController(this, actionManager_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::initListeners()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::hit(const HitInfo& _hitInfo)
{
	Player::hit(_hitInfo);

	if (_hitInfo.AttackDataInfo->isFallDownAttack_)
	{
		playBaseActionForce(BaseAction::FallDown);
		return;
	}

	playBaseActionForce(BaseAction::Hit);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::removeActionManager()
{
	JCORE_DELETE_SAFE(actionManager_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::removeController()
{
	JCORE_DELETE_SAFE(controller_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::update(float _dt)
{
	Player::update(_dt);

	if (controller_)
		controller_->update(_dt);

	if (actionManager_)
		actionManager_->update(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::onKeyPressed(SGEventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent)
{
	if (controller_)
		controller_->onKeyPressed(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::onKeyReleased(SGEventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent)
{
	if (controller_)
		controller_->onKeyReleased(_keyCode, _pEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	Player::onFrameBegin(_pAnimation, _pTexture);

	if (actionManager_)
		actionManager_->onFrameBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	Player::onFrameEnd(_pAnimation, _pTexture);

	if (actionManager_)
		actionManager_->onFrameEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	Player::onAnimationBegin(_pAnimation, _pTexture);

	if (actionManager_)
		actionManager_->onAnimationBegin(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	Player::onAnimationEnd(_pAnimation, _pTexture);

	if (actionManager_)
		actionManager_->onAnimationEnd(_pAnimation, _pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::playAction(int _actionCode)
{
	actionManager_->runAction(_actionCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::playActionForce(int _actionCode)
{
	actionManager_->stopActionForce();
	actionManager_->runAction(_actionCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::playBaseActionForce(BaseAction_t _baseActionType)
{
	actionManager_->stopActionForce();
	actionManager_->runBaseAction(_baseActionType);
}

//////////////////////////////////////////////////////////////////////////////////////////
void HostPlayer::playBaseAction(BaseAction_t _baseAction)
{
	actionManager_->runBaseAction(_baseAction);
}

//////////////////////////////////////////////////////////////////////////////////////////
int HostPlayer::getRunningActionCode()
{
	SGAction* pAction = actionManager_->getRunningAction();
	if (pAction == nullptr)
		return InvalidValue_v;
	return pAction->getActionCode();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActionMgr* HostPlayer::actionManager()
{
	DebugAssertMsg(actionManager_, "액션 매니저가 세팅되지 않았습니다.");
	return actionManager_;
}

//////////////////////////////////////////////////////////////////////////////////////////
PlayerController* HostPlayer::ctrl()
{
	DebugAssertMsg(controller_, "플레이어 컨트롤러가 세팅되지 않았습니다.");
	return controller_;
}

//////////////////////////////////////////////////////////////////////////////////////////
