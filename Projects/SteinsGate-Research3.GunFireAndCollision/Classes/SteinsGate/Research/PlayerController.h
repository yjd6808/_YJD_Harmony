/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:00 AM
 * =====================
 *
 * 움직임에 대한 구분은 2가지로 나눔
 *  1. 스킬을 시전하여 액션 수행중에 위치 이동만 가능한지
 *	2. 스킬이 시전중이 아니라서 위치 이동과 더불어 걷기, 평상시 상태로의 캐릭터 모션전환이 가능한 경우
 *	(runningAction == nullptr인 경우)
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Research/PlayerActionManager.h>
#include <SteinsGate/Research/CharacterSprite.h>

#include <JCore/Time.h>

class Player;
class PlayerController : public cocos2d::Ref
{
public:
	// 키와 그 키를 입력한 시간
	struct InputWithTime {
		JCore::DateTime Time{};
		ControlKey_t ControlKey{};
	};

	static PlayerController* create(Player* player, CharacterSprite* character, PlayerActionManager* actionManager);

	PlayerController(Player* player, CharacterSprite* character, PlayerActionManager* actionManager)
		: m_pActor(player)
		, m_pCharacter(character)
		, m_pActionManager(actionManager) {}
	~PlayerController();

	void init();
	void update(float delta);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyPressed(ControlKey_t pressedKey);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(ControlKey_t pressedKey);

	SpriteDirection_t getSpriteDirection() { return m_eDir; }
	ControlKey_t getLastestReleasedKey() { return m_LastestReleasedKey.ControlKey; }
	ControlKey_t getLastestPressedKey() { return m_LastestPressedKey.ControlKey; }
	ControlKey_t convertControlKey(cocos2d::EventKeyboard::KeyCode keyCode);
	
	bool isKeyPressed(ControlKey_t controlKey);
	bool isMoveKeyPressed();
	bool isHorizontalKeyPressed();	// 우측, 왼쪽 키가 모두 눌린 경우
	bool isVerticalKeyPressed();	// 위  , 아래 키가 모두 눌린 경우
	bool isFreezedWalkState(ControlKey_t releasedKey);
	bool hasRunningAction() { return m_pActionManager->hasRunningAction(); }	// 달리는 액션이 아니라 실행중인 액션이란 뜻
	bool hasPreviousAction() { return m_pActionManager->hasPreviousAction(); }
	bool fixFreezedWalkState(ControlKey_t releasedKey);

	bool canUseCommand() { return m_bCabUseCommand; }
	bool cannotUseCommand() { return m_bCabUseCommand == false; }

	void checkComboSequence();
	void fixReverseKeyPressed(ControlKey_t pressedKey);


	void idle();
	void walk();

	void updateRun(float dt);
	void updateWalking(float dt);
	void updateDirection(ControlKey_t pressedKey);
	void reflectPressedMoveKeys();	// 액션 수행동안 키 입력을 무시하는데 그사이 눌린 키들에 대한 처리
	void setCommandable(bool commandable) { m_bCabUseCommand = commandable; }

	
private:
	// ==================================================
	// 주입 필드
	// ==================================================
	Player* m_pActor;
	CharacterSprite* m_pCharacter;
	PlayerActionManager* m_pActionManager;


	// ==================================================
	// 기본 필드
	// ==================================================
	ControlKey_t m_CocosKeyCodeToControlKeyMap[200];			// 코코스 키를 ControlKey로 변환 (None(0)으로 초기화)
	int m_ControlKeyPressedMap[ControlKey::Max]{};
	InputWithTime m_LastestPressedKey;			// 제일 최근에 누른 키
	InputWithTime m_LastestReleasedKey;			// 제일 최근에 땐 키
	InputWithTime m_ControlKeySequence[ComboSequenceCount_v]{}; // 맨 앞이 제일 최근에 입력한 키

	bool m_bCabUseCommand{};
	SpriteDirection_t m_eDir = SpriteDirection::Right;
	
};



