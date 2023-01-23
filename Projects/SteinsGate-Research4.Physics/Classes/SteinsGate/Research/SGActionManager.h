/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 9:00:34 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Research/SGComboTree.h>

class SGFrameTexture;
class SGPlayer;
class SGPlayerController;
class SGActionManager
{
public:
	SGActionManager(SGPlayer* player);
	~SGActionManager();

	void init(int characterType);
	void initGunnerActions();
	void update(float dt);
	void onKeyPressed(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	void onKeyReleased(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode);
	void onKeyPressedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode);		// 키가 눌려진 상태가 수정되기전 호출
	void onKeyReleasedBefore(SGPlayerController* controller, SGEventKeyboard::KeyCode keyCode);	// 키가 안눌려진 상태가 수정되기전 호출
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);

	
	bool canRunAction();
	void setRunningAction(SGAction* action);

	bool hasPreviousAction() { return m_pPreviousAction != nullptr; }
	bool hasRunningAction() { return m_pRunningAction != nullptr; }
	bool isBaseActionRunning(BaseAction_t baseActionType);
	bool isActionRunning(int actionCode);
	bool isRunnningActionIdleOrWalkAction();
	bool canRunningActionChangeDirection();
	

	SGAction* getRunningAction() { return m_pRunningAction; }
	SGAction* getPreviousAction() { return m_pPreviousAction; }
	SGAction* getAction(int actionCode);
	SGAction* getBaseAction(BaseAction_t baseActionType);
	SGComboAction* getComboAction(const SGComboKeyList& keys) { return m_ComboTree.GetComboAction(keys); }
	void runAction(int actionCode);
	void runAction(SGAction* action);
	void runBaseAction(BaseAction_t baseActionType);
private:
	SGPlayer* m_pPlayer;
	SGComboTree m_ComboTree;
	SGAction* m_pRunningAction;					// 캐릭터 자체가 움직여서 사용하는 액션
	SGAction* m_pPreviousAction;
	SGAction* m_pBaseAction[BaseAction::Max];
	SGHashMap<int, SGAction*> m_ActionMap;
};
