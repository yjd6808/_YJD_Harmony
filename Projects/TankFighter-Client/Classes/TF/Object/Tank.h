#pragma once

#include <TF/SourceHeader.h>

#include <Common/Enum.h>

#include <JCore/Container/HashMap.h>


class Tank : public Scale9Sprite
{
public:
	Tank(int characterUID, Layer* activeLayer) : m_iCharacterUID(characterUID), m_pActiveLayer(activeLayer) {}
	bool init2();
	static Tank* create(int characterUID, Layer* playingScene);	// 이 탱크가 활동하는 레이어를 지정해준다.

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

	// UpdateTankMove() 함수가 호출되면 true로됨
	bool IsUpdated() const										{ return m_bUpdated;}
	TankMove& GetMove()											{ return m_TankMove; }
private:
	int m_iCharacterUID;			// 이 탱크를 모는놈
	bool m_bMoveable = false;		// 움직일 수 있는지.
	bool m_bFireable = false;		// 쏠 수 있는지
	bool m_bUpdated = false;		// 업데이트 되었는지

	Layer* m_pActiveLayer = nullptr;
	Sprite* m_pBodyCollidors[2] = { nullptr, nullptr };
	Scale9Sprite* m_pTower = nullptr;
	Scale9Sprite* m_pGun = nullptr;

	float m_fPrevRot = 0.0f;			// 다음 틱에서 변경될 수 있는 회전
	Vec2 m_PrevPos;						// 다음 틱에서 변경될 수 있는 위치

	TankMove m_TankMove{
		0.0, 0.0,
		TANK_MOVE_SPEED,
		static_cast<Int8>(MoveDirection::None),
		static_cast<Int8>(RotateDirection::None),
		0.0f,
		TANK_ROTATION_SPEED };
	HashMap<EventKeyboard::KeyCode, bool> m_KeyPressedMap;
};

