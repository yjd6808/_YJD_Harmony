/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:48:30 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/Player.h>
#include <SteinsGate/Client/PlayerController.h>
#include <SteinsGate/Client/ActionMgr.h>


class HostPlayer
	: public Player
	  , public JCore::SingletonPointer<HostPlayer>
{
private:
	friend class TSingleton;
	HostPlayer();
	~HostPlayer() override;

public:
	void initialize() override;
	void initActionManager();
	void initController();
	void initListeners() override;
	void hit(const HitInfo& _hitInfo) override;

	void removeActionManager();
	void removeController();

	void update(float _dt) override;
	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent);
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent);
	void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture) override;

	void playAction(int _actionCode);
	void playActionForce(int _actionCode);
	void playBaseActionForce(BaseAction_t _baseActionType);
	void playBaseAction(BaseAction_t _baseAction);

	int getRunningActionCode();

	ActionMgr* actionManager();
	PlayerController* ctrl();
	AccountData& accountData() { return accountData_; }

private:
	AccountData accountData_;
	ActionMgr* actionManager_;
	PlayerController* controller_;
};
