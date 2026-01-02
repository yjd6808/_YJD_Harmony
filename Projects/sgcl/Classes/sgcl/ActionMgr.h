/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:00:34 AM
 * =====================
 *
 */


#pragma once


#include <sgcl/ComboTree.h>

class FrameTexture;
class HostPlayer;
class PlayerController;

class ActionMgr
{
public:
	ActionMgr(HostPlayer* _pPlayer);
	~ActionMgr();

	void Init(int _charType);
	void InitGunnerActions();

	void Update(float _dt);
	void OnKeyPressed(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode);
	void OnKeyReleased(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode);
	void OnKeyPressedBefore(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode); // 키가 눌려진 상태가 수정되기전 호출
	void OnKeyReleasedBefore(PlayerController* _pController, cc::EventKeyboard::KeyCode _keyCode); // 키가 안눌려진 상태가 수정되기전 호출
	void OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	void OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);

	bool CanRunAction();
	bool HasPreviousAction() { return previousAction_ != nullptr; }
	bool HasRunningAction() { return runningAction_ != nullptr; }
	bool IsBaseActionRunning(BaseAction_t _baseActionType);
	bool IsActionRunning(int _actionCode);
	bool IsRunnningActionIdleOrWalkAction();

	SGAction* GetRunningAction() { return runningAction_; }
	SGAction* GetPreviousAction() { return previousAction_; }
	SGAction* GetAction(int _actionCode);
	SGAction* GetBaseAction(BaseAction_t _baseActionType);
	SGAction* GetComboAction(const ComboKeyList& _keys) { return comboTree_.GetComboAction(_keys); }
	void RunAction(int _actionCode);
	void RunAction(SGAction* _pAction);
	void RunBaseAction(BaseAction_t _baseActionType);

	void StopActionForce();

private:
	HostPlayer* player_;
	SGComboTree comboTree_;
	SGAction* runningAction_; // 캐릭터 자체가 움직여서 사용하는 액션
	SGAction* previousAction_;
	SGAction* baseAction_[BaseAction::Max];
	jc::HashMap<int, SGAction*> actionMap_;
};
