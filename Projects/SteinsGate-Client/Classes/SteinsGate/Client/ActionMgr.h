/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:00:34 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/ComboTree.h>

class FrameTexture;
class HostPlayer;
class PlayerController;

class ActionMgr
{
public:
	ActionMgr(HostPlayer* _pPlayer);
	~ActionMgr();

	void init(int _charType);
	void initGunnerActions();
	void update(float _dt);
	void onKeyPressed(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode);
	void onKeyReleased(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode);
	void onKeyPressedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode); // 키가 눌려진 상태가 수정되기전 호출
	void onKeyReleasedBefore(PlayerController* _pController, SGEventKeyboard::KeyCode _keyCode); // 키가 안눌려진 상태가 수정되기전 호출
	void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);

	bool canRunAction();
	bool hasPreviousAction() { return previousAction_ != nullptr; }
	bool hasRunningAction() { return runningAction_ != nullptr; }
	bool isBaseActionRunning(BaseAction_t _baseActionType);
	bool isActionRunning(int _actionCode);
	bool isRunnningActionIdleOrWalkAction();

	SGAction* getRunningAction() { return runningAction_; }
	SGAction* getPreviousAction() { return previousAction_; }
	SGAction* getAction(int _actionCode);
	SGAction* getBaseAction(BaseAction_t _baseActionType);
	SGAction* getComboAction(const ComboKeyList& _keys) { return comboTree_.getComboAction(_keys); }
	void runAction(int _actionCode);
	void runAction(SGAction* _pAction);
	void runBaseAction(BaseAction_t _baseActionType);

	void stopActionForce();

private:
	HostPlayer* player_;
	SGComboTree comboTree_;
	SGAction* runningAction_; // 캐릭터 자체가 움직여서 사용하는 액션
	SGAction* previousAction_;
	SGAction* baseAction_[BaseAction::Max];
	SGHashMap<int, SGAction*> actionMap_;
};
