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
	ActionMgr(HostPlayer* player);
	~ActionMgr();

	void init(int charType);
	void initGunnerActions();
	void update(float dt);
	void onKeyPressed(PlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	void onKeyReleased(PlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	void onKeyPressedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode);		// 키가 눌려진 상태가 수정되기전 호출
	void onKeyReleasedBefore(PlayerController* controller, SGEventKeyboard::KeyCode keyCode);	// 키가 안눌려진 상태가 수정되기전 호출
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture);
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture);

	bool canRunAction();
	bool hasPreviousAction() { return m_pPreviousAction != nullptr; }
	bool hasRunningAction() { return m_pRunningAction != nullptr; }
	bool isBaseActionRunning(BaseAction_t baseActionType);
	bool isActionRunning(int actionCode);
	bool isRunnningActionIdleOrWalkAction();
	
	SGAction* getRunningAction() { return m_pRunningAction; }
	SGAction* getPreviousAction() { return m_pPreviousAction; }
	SGAction* getAction(int actionCode);
	SGAction* getBaseAction(BaseAction_t baseActionType);
	SGAction* getComboAction(const ComboKeyList& keys) { return m_ComboTree.getComboAction(keys); }
	void runAction(int actionCode);
	void runAction(SGAction* action);
	void runBaseAction(BaseAction_t baseActionType);

	void stopActionForce();
private:
	HostPlayer* m_pPlayer;
	SGComboTree m_ComboTree;
	SGAction* m_pRunningAction;					// 캐릭터 자체가 움직여서 사용하는 액션
	SGAction* m_pPreviousAction;
	SGAction* m_pBaseAction[BaseAction::Max];
	SGHashMap<int, SGAction*> m_ActionMap;
};
