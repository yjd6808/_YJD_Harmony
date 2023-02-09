/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:00 AM
 * =====================
 *
 */


#pragma once



#include <SteinsGate/Research/ComboTree.h>

#include <SteinsGate/Common/Engine.h>
#include <SteinsGate/Research/CharacterSprite.h>

#include <JCore/Time.h>

class PlayerController : public cocos2d::Node
{
public:
	// 키와 그 키를 입력한 시간
	struct InputWithTime
	{
		JCore::DateTime Time{};
		ControlKey_t ControlKey{};
	};

	enum State {
		Idle,
		Run,
		Walk,
		Sliding
	};

	static PlayerController* create(CharacterSprite* character);

	PlayerController(CharacterSprite* character)
		: m_pCharacter(character) {}

	bool init() override;
	void update(float delta);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyPressed(ControlKey_t pressedKey);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(ControlKey_t pressedKey);
	
	
	void onFrameBegin(int frameIndex, int frameIndexInAnimation);

	bool isKeyPressed(ControlKey_t controlKey);
	bool isMoveKeyPressed();

	void checkComboSequence();
	void checkReverseKeyPressed(ControlKey_t pressedKey);
	void checkFreezeWalkState(ControlKey_t controlKey);

	void idle();
	void walk();
	void run(SpriteDirection_t direction);
	void slide(SpriteDirection_t direction);

	void updateRun(float dt);
	void updateWalking(float dt);
	void updateDirection(ControlKey_t pressedKey);
	void reflectPressedMoveKeys();	// 액션 수행동안 키 입력을 무시하는데 그사이 눌린 키들에 대한 처리

	bool isIdle()		{ return m_eState == State::Idle; }
	bool isRunning()	{ return m_eState == State::Run; }
	bool isWalking()	{ return m_eState == State::Walk; }
	bool isSliding()	{ return m_eState == State::Sliding; }
	
private:
	CharacterSprite* m_pCharacter;
	ComboActionPtr m_spRunningAction;
	ComboTree m_ComboTree;
	ControlKey_t m_CocosKeyCodeToControlKeyMap[200]{};			// 코코스 키를 ControlKey로 변환 (None(0)으로 초기화)
	int m_ControlKeyPressedMap[ControlKey::Max]{};
	InputWithTime m_eLastestPressedKey;		// 제일 최근에 누른 키
	InputWithTime m_eLastestReleasedKey;	// 제일 최근에 땐 키
	InputWithTime m_ControlKeySequence[ComboSequenceCount_v]{}; // 맨 앞이 제일 최근에 입력한 키

	bool m_bPreventMove{};
	State m_eState;
};



