#pragma once

#include <TF/SourceHeader.h>

#include <Common/Enum.h>

#include <JCore/Container/HashMap.h>


class Tank : public Scale9Sprite
{
public:
	bool init2();
	static Tank* create();

	void updateRotation(float delta);
	void updatePosition(float delta);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	JCore::Vector<Sprite*> GetColliders() const { return { m_pBodyCollidors[0], m_pBodyCollidors[1] }; }
	Vec2 GetPrevPos() const { return m_PrevPos; }
	float GetPrevRotation() const { return m_fPrevRot; }
private:
	Sprite* m_pBodyCollidors[2] = { nullptr, nullptr };
	Scale9Sprite* m_pTower = nullptr;
	Scale9Sprite* m_pGun = nullptr;

	float m_fTankRotationSpeed = 70.0f;	// 탱크 회전 속도
	float m_fTankMoveSpeed = 70.0f;		// 탱크 움직이는 속도
	bool m_bPreventMove = false;		// 움직

	float m_fPrevRot = 0.0f;			// 다음 틱에서 변경될 수 있는 회전
	Vec2 m_PrevPos;						// 다음 틱에서 변경될 수 있는 위치

	MoveDirection m_eMoveDir = MoveDirection::None;
	RotateDirection m_eRotateDir = RotateDirection::None;

	JCore::HashMap<EventKeyboard::KeyCode, bool> m_KeyPressedMap;
};

