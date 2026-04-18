/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:33 AM
 * =====================
 *
 */

#pragma once

#include "sg/Struct/SteinsGate_Action.h"

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
		jc::DateTime time_{};
		ControlKey_t controlKey_{};
	};

	PlayerController(HostPlayer* _pPlayer, ActionMgr* _pActionManager);
	~PlayerController();

	static PlayerController* Create(HostPlayer* _pPlayer, ActionMgr* _pActionManager);

	void Init();
	void Update(float _delta);
	void OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent);
	void OnKeyPressed(ControlKey_t _pressedKey);
	void OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent);
	void OnKeyReleased(ControlKey_t _releasedKey);

	SpriteDirection_t GetSpriteDirection();
	ControlKey_t GetLastestReleasedKey() { return lastestReleasedKey_.controlKey_; }
	ControlKey_t GetLastestPressedKey() { return lastestPressedKey_.controlKey_; }
	ControlKey_t ConvertControlKey(cc::EventKeyboard::KeyCode _keyCode);

	bool IsKeyPressed(ControlKey_t _controlKey);
	bool IsMoveKeyPressed();

	bool CanUseCommand() { return cabUseCommand_; }
	bool CannotUseCommand() { return cabUseCommand_ == false; }
	void CheckComboSequence();

	void Idle();
	void Walk();

	void UpdateMove(float _delta);
	void UpdateDirection(ControlKey_t _pressedKey);
	void ReflectPressedMoveKeys(); // 액션 수행동안 키 입력을 무시하는데 그사이 눌린 키들에 대한 처리
	void SetCommandable(bool _commandable) { cabUseCommand_ = _commandable; }

private:
	HostPlayer* player_;
	ActionMgr* actionManager_;

	// ==================================================
	// 기본 필드
	// ==================================================
	jc::Vector<ControlKey_t> pressedArrowKeyState_;
	ControlKey_t cocosKeyCodeToControlKeyMap_[200]; // 코코스 키를 ControlKey로 변환 (None(0)으로 초기화)
	bool controlKeyPressedMap_[ControlKey::Max]{};
	InputTime lastestPressedKey_; // 제일 최근에 누른 키
	InputTime lastestReleasedKey_; // 제일 최근에 땐 키
	InputTime controlKeySequence_[Const::Action::ComboSequenceCount]{}; // 맨 앞이 제일 최근에 입력한 키
	bool cabUseCommand_{};
};
