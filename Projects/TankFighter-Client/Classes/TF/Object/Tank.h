#pragma once

#include <TF/SourceHeader.h>

#include <Common/Enum.h>

#include <JCore/Container/HashMap.h>

class Bullet;
class Tank : public Scale9Sprite
{
public:
	Tank(int characterUID, Layer* activeLayer) : m_pActiveLayer(activeLayer) { SetCharacterUID(characterUID); }
	bool init2();
	static Tank* create(int characterUID, Layer* playingScene);	// �� ��ũ�� Ȱ���ϴ� ���̾ �������ش�.

	void updateRotation(float delta);
	void updatePosition(float delta);
	void updateShotDelay(float delta);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void setMoveable(bool enabled)								{ m_bMoveable = enabled; }
	void setFireable(bool enabled)								{ m_bFireable = enabled; }

	bool IsMoveable() const										{ return m_bMoveable; }
	bool IsFireable() const										{ return m_bFireable; }

	JCore::Vector<Sprite*> GetColliders() const					{ return { m_pBodyCollidors[0], m_pBodyCollidors[1] }; }
	void UpdateTankMove(TankMove& move);
	bool HasOwner() const										{ return m_TankMove.CharacterUID != INVALID_UID; }
	void SetCharacterUID(int characterUID)						{ m_TankMove.CharacterUID = characterUID; }
	int	 GetCharacterUID() const								{ return m_TankMove.CharacterUID; }

	// UpdateTankMove() �Լ��� ȣ��Ǹ� true�ε�
	bool IsUpdated() const										{ return m_bUpdated;}
	TankMove& GetMove()											{ return m_TankMove; }
	void SetFireCallback(JCore::Action<Bullet*> callback)		{ m_FilreCallBack = callback; }
	void Fire();
private:
	bool m_bMoveable = false;		// ��ũ�� Ű����� ������ �� �ִ���
	bool m_bFireable = false;		// ��ũ�� Ű���带 ������ ���� �� �� �ִ���
	bool m_bUpdated = false;		// ������Ʈ �Ǿ�����

	Layer* m_pActiveLayer = nullptr;
	Sprite* m_pBodyCollidors[2] = { nullptr, nullptr };
	Scale9Sprite* m_pTower = nullptr;
	Scale9Sprite* m_pGun = nullptr;
	Node* m_pFirePos;

	float m_fShotDelay = 0.0f;			// �� ��� �ִ�
	bool m_bShotable = false;			// �����̽ð��� ����ؼ� ���� ���� �� �� �ִ���
	JCore::Action<Bullet*> m_FilreCallBack = nullptr; // �� �߻��Ҷ����� ȣ���� �ݹ� �Լ�

	TankMove m_TankMove{
		INVALID_UID,
		false,
		0.0, 0.0,
		TANK_MOVE_SPEED,
		MoveDirection::None,
		RotateDirection::None,
		0.0f,
		TANK_ROTATION_SPEED };
	HashMap<EventKeyboard::KeyCode, bool> m_KeyPressedMap;
};

