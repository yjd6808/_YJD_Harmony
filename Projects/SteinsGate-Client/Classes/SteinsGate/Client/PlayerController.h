/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:33 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/Tutturu.h>

#include <JCore/Time.h>

struct MapAreaInfo;

class HostPlayer;
class Character;
class MapLayer;
class ActionMgr;

class PlayerController
{
public:
	// 키와 그 키를 입력한 시간
	struct InputTime
	{
		JCore::DateTime Time{};
		ControlKey_t ControlKey{};
	};

	PlayerController(HostPlayer* _pPlayer, ActionMgr* _pActionManager);
	~PlayerController();

	static PlayerController* create(HostPlayer* _pPlayer, ActionMgr* _pActionManager);

	void init();
	void update(float _delta);
	void onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent);
	void onKeyPressed(ControlKey_t _pressedKey);
	void onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent);
	void onKeyReleased(ControlKey_t _releasedKey);

	SpriteDirection_t getSpriteDirection();
	ControlKey_t getLastestReleasedKey() { return lastestReleasedKey_.ControlKey; }
	ControlKey_t getLastestPressedKey() { return lastestPressedKey_.ControlKey; }
	ControlKey_t convertControlKey(SGEventKeyboard::KeyCode _keyCode);

	bool isKeyPressed(ControlKey_t _controlKey);
	bool isMoveKeyPressed();

	bool canUseCommand() { return cabUseCommand_; }
	bool cannotUseCommand() { return cabUseCommand_ == false; }
	void checkComboSequence();

	void idle();
	void walk();

	void updateMove(float _delta);
	void updateDirection(ControlKey_t _pressedKey);
	void reflectPressedMoveKeys(); // 액션 수행동안 키 입력을 무시하는데 그사이 눌린 키들에 대한 처리
	void setCommandable(bool _commandable) { cabUseCommand_ = _commandable; }

private:
	HostPlayer* player_;
	ActionMgr* actionManager_;

	// ==================================================
	// 기본 필드
	// ==================================================
	SGVector<ControlKey_t> pressedArrowKeyState_;
	ControlKey_t cocosKeyCodeToControlKeyMap_[200]; // 코코스 키를 ControlKey로 변환 (None(0)으로 초기화)
	bool controlKeyPressedMap_[ControlKey::Max]{};
	InputTime lastestPressedKey_; // 제일 최근에 누른 키
	InputTime lastestReleasedKey_; // 제일 최근에 땐 키
	InputTime controlKeySequence_[Const::Action::ComboSequenceCount]{}; // 맨 앞이 제일 최근에 입력한 키
	bool cabUseCommand_{};
};
