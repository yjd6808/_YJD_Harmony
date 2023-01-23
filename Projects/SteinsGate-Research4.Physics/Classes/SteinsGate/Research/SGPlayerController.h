/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:33 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/Config.h>
#include <SteinsGate/Research/SGActionManager.h>

#include <JCore/Time.h>

#include "SGCharacter.h"

class SGPlayer;
class SGCharacter;
class SGActionManager;
class SGPlayerController
{
public:
	// 키와 그 키를 입력한 시간
	struct InputTime {
		JCore::DateTime Time{};
		ControlKey_t ControlKey{};
	};

	SGPlayerController(SGPlayer* player, SGCharacter* character, SGActionManager* actionManager);
	~SGPlayerController();

	static SGPlayerController* create(SGPlayer* player, SGCharacter* character, SGActionManager* actionManager);

	void init();
	void update(float delta);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyPressed(ControlKey_t pressedKey);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(ControlKey_t pressedKey);

	SpriteDirection_t getSpriteDirection() { return m_pCharacter->getActorSprite()->getSpriteDirection(); }
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

	bool canUseCommand() { return m_bCabUseCommand; }
	bool cannotUseCommand() { return m_bCabUseCommand == false; }
	void checkComboSequence();

	void idle();
	void walk();

	void updateWalking(float dt);
	void updateDirection(ControlKey_t pressedKey);
	void reflectPressedMoveKeys();	// 액션 수행동안 키 입력을 무시하는데 그사이 눌린 키들에 대한 처리
	void setCommandable(bool commandable) { m_bCabUseCommand = commandable; }
private:
	SGPlayer* m_pPlayer;
	SGCharacter* m_pCharacter;
	SGActionManager* m_pActionManager;

	// ==================================================
	// 기본 필드
	// ==================================================
	ControlKey_t m_CocosKeyCodeToControlKeyMap[200];		// 코코스 키를 ControlKey로 변환 (None(0)으로 초기화)
	int m_ControlKeyPressedMap[ControlKey::Max]{};
	InputTime m_LastestPressedKey;							// 제일 최근에 누른 키
	InputTime m_LastestReleasedKey;							// 제일 최근에 땐 키
	InputTime m_ControlKeySequence[ComboSequenceCount_v]{}; // 맨 앞이 제일 최근에 입력한 키
	bool m_bCabUseCommand{};
};

