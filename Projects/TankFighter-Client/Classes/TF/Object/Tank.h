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

	float m_fTankRotationSpeed = 70.0f;	// ��ũ ȸ�� �ӵ�
	float m_fTankMoveSpeed = 70.0f;		// ��ũ �����̴� �ӵ�
	bool m_bPreventMove = false;		// ����

	float m_fPrevRot = 0.0f;			// ���� ƽ���� ����� �� �ִ� ȸ��
	Vec2 m_PrevPos;						// ���� ƽ���� ����� �� �ִ� ��ġ

	MoveDirection m_eMoveDir = MoveDirection::None;
	RotateDirection m_eRotateDir = RotateDirection::None;

	JCore::HashMap<EventKeyboard::KeyCode, bool> m_KeyPressedMap;
};

