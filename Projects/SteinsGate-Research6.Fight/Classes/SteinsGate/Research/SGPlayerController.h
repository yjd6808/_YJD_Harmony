/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 10:25:33 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Research/SGConfig.h>

#include <JCore/Time.h>

struct SGMapInfo;

class SGHostPlayer;
class SGCharacter;
class SGMapLayer;
class SGActionManager;
class SGPlayerController
{
public:
	// 키와 그 키를 입력한 시간
	struct InputTime {
		JCore::DateTime Time{};
		ControlKey_t ControlKey{};
	};

	SGPlayerController(SGHostPlayer* player, SGCharacter* character, SGActionManager* actionManager);
	~SGPlayerController();

	static SGPlayerController* create(SGHostPlayer* player, SGCharacter* character, SGActionManager* actionManager);

	void init();
	void update(float delta);
	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event);
	void onKeyPressed(ControlKey_t pressedKey);
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event);
	void onKeyReleased(ControlKey_t pressedKey);

	SpriteDirection_t getSpriteDirection();
	ControlKey_t getLastestReleasedKey() { return m_LastestReleasedKey.ControlKey; }
	ControlKey_t getLastestPressedKey() { return m_LastestPressedKey.ControlKey; }
	ControlKey_t convertControlKey(SGEventKeyboard::KeyCode keyCode);

	bool isKeyPressed(ControlKey_t controlKey);
	bool isMoveKeyPressed();

	bool canUseCommand() { return m_bCabUseCommand; }
	bool cannotUseCommand() { return m_bCabUseCommand == false; }
	void checkComboSequence();

	void idle();
	void walk();

	void updateMove(float dt);
	void updateLeftMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& thicknessRect);
	void updateRightMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& nextThicknessRect);
	void updateUpMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& nextThicknessRect);
	void updateDownMove(SGMapLayer* mapLayer, SGMapInfo* mapInfo, const SGRect& nextThicknessRect);
	void updateDirection(ControlKey_t pressedKey);
	void reflectPressedMoveKeys();	// 액션 수행동안 키 입력을 무시하는데 그사이 눌린 키들에 대한 처리
	void setCommandable(bool commandable) { m_bCabUseCommand = commandable; }
private:
	SGHostPlayer* m_pPlayer;
	SGCharacter* m_pCharacter;
	SGActionManager* m_pActionManager;

	// ==================================================
	// 기본 필드
	// ==================================================
	ControlKey_t m_CocosKeyCodeToControlKeyMap[200];		// 코코스 키를 ControlKey로 변환 (None(0)으로 초기화)
	bool m_ControlKeyPressedMap[ControlKey::Max]{};
	InputTime m_LastestPressedKey;							// 제일 최근에 누른 키
	InputTime m_LastestReleasedKey;							// 제일 최근에 땐 키
	InputTime m_ControlKeySequence[ComboSequenceCount_v]{}; // 맨 앞이 제일 최근에 입력한 키
	bool m_bCabUseCommand{};
};

