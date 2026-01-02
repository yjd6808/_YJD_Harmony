/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:48:30 AM
 * =====================
 *
 */


#pragma once


#include <sgcl/Player.h>
#include <sgcl/PlayerController.h>
#include <sgcl/ActionMgr.h>


class HostPlayer
	: public Player
	  , public jc::SingletonPointer<HostPlayer>
{
private:
	friend class TSingleton;
	HostPlayer();
	~HostPlayer() override;

public:
	void Initialize() override;
	void InitActionManager();
	void InitController();
	void InitListeners() override;
	void Hit(const HitInfo& _hitInfo) override;

	void RemoveActionManager();
	void RemoveController();

	void update(float _dt) override;

	void OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent);
	void OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent);
	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;

	void PlayAction(int _actionCode);
	void PlayActionForce(int _actionCode);
	void PlayBaseActionForce(BaseAction_t _baseActionType);
	void PlayBaseAction(BaseAction_t _baseAction);

	int GetRunningActionCode();

	ActionMgr*			GetActionManager();
	PlayerController*	GetController();
	AccountData&		GetAccountData() { return accountData_; }

private:
	AccountData			accountData_;
	ActionMgr*			pActionManager_;
	PlayerController*	pController_;
};
