/*
 * 작성자: 윤정도
 * 생성일: 1/14/2023 12:51:30 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/ComboTree.h>


class Player;
class PlayerActionManager : public cocos2d::Ref {
public:
	static PlayerActionManager* create(Player* player);

	PlayerActionManager(Player* player);
	~PlayerActionManager() override;

	void init();
	void update(float dt);
	void onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);
	void onKeyReleased(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);
	void onKeyPressedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);		// 키가 눌려진 상태가 수정되기전 호출
	void onKeyReleasedBefore(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode);	// 키가 안눌려진 상태가 수정되기전 호출
	void onFrameBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	void onFrameEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	void onAnimateBegin(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);
	void onAnimateEnd(CharacterSprite* character, CharacterPartAnimate* animate, CharacterPartSpriteFrame* frame);

	bool canRunAction();
	void setRunningAction(ActionAbstract* action);

	bool hasPreviousAction() { return m_pPreviousAction != nullptr; }
	bool hasRunningAction() { return m_pRunningAction != nullptr; }
	ActionAbstract* getRunningAction() { return m_pRunningAction; }
	ActionAbstract* getPreviousAction() { return m_pPreviousAction; }
	ActionAbstract* getAction(ActionType_t type) ;
	ComboAction* getComboAction(const ComboKeyList& keys) { return m_ComboTree.GetComboAction(keys); }
	void runAction(ActionType_t type);
	void runAction(ActionAbstract* action);
	void runComboAction(const ComboKeyList& keys);
	
private:
	Player* m_pPlayer;
	ComboTree m_ComboTree;
	ActionAbstract* m_pRunningAction{};		// 캐릭터 자체가 움직여서 사용하는 액션
	ActionAbstract* m_pPreviousAction{};
	JCore::HashMap<ActionType_t, ActionAbstract*> m_Map;
	
};

