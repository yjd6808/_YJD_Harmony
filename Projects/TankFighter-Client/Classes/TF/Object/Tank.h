#pragma once

#include <TF/SourceHeader.h>

#include <Common/Enum.h>

#include <JCore/Container/HashMap.h>


class Tank : public Scale9Sprite
{
public:
	Tank(int characterUID, Layer* activeLayer) : m_iCharacterUID(characterUID), m_pActiveLayer(activeLayer) {}
	bool init2();
	static Tank* create(int characterUID, Layer* playingScene);	// �� ��ũ�� Ȱ���ϴ� ���̾ �������ش�.

	void updateRotation(float delta);
	void updatePosition(float delta);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void setMoveable(bool enabled)								{ m_bMoveable = enabled; }
	void setFireable(bool enabled)								{ m_bFireable = enabled; }

	bool IsMoveable() const										{ return m_bMoveable; }
	bool IsFireable() const										{ return m_bFireable; }

	JCore::Vector<Sprite*> GetColliders() const					{ return { m_pBodyCollidors[0], m_pBodyCollidors[1] }; }
	Vec2 GetPrevPos() const { return m_PrevPos; }
	float GetPrevRotation() const { return m_fPrevRot; }

	void UpdateTankMove(TankMove& move);
	bool HasOwner() const										{ return m_iCharacterUID != INVALID_UID; }
	void SetCharacterUID(int characterUID)						{ m_iCharacterUID = characterUID; }
	int	 GetCharacterUID() const								{ return m_iCharacterUID; }

	// UpdateTankMove() �Լ��� ȣ��Ǹ� true�ε�
	bool IsUpdated() const										{ return m_bUpdated;}
	TankMove& GetMove()											{ return m_TankMove; }
private:
	int m_iCharacterUID;			// �� ��ũ�� ��³�
	bool m_bMoveable = false;		// ������ �� �ִ���.
	bool m_bFireable = false;		// �� �� �ִ���
	bool m_bUpdated = false;		// ������Ʈ �Ǿ�����

	Layer* m_pActiveLayer = nullptr;
	Sprite* m_pBodyCollidors[2] = { nullptr, nullptr };
	Scale9Sprite* m_pTower = nullptr;
	Scale9Sprite* m_pGun = nullptr;

	float m_fPrevRot = 0.0f;			// ���� ƽ���� ����� �� �ִ� ȸ��
	Vec2 m_PrevPos;						// ���� ƽ���� ����� �� �ִ� ��ġ

	TankMove m_TankMove{
		0.0, 0.0,
		TANK_MOVE_SPEED,
		static_cast<Int8>(MoveDirection::None),
		static_cast<Int8>(RotateDirection::None),
		0.0f,
		TANK_ROTATION_SPEED };
	HashMap<EventKeyboard::KeyCode, bool> m_KeyPressedMap;
};

