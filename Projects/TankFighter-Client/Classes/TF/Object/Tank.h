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
	static Tank* create(int characterUID, Layer* playingScene);	// 이 탱크가 활동하는 레이어를 지정해준다.

	void updateRotation(float delta);
	void updatePosition(float delta);
	void updateShotDelay(float delta);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void setMoveable(bool enabled)								{ m_bMoveable = enabled; }
	void setFireable(bool enabled)								{ m_bFireable = enabled; }

	bool IsMoveable() const										{ return m_bMoveable; }
	bool IsFireable() const										{ return m_bFireable; }

	JCore::Vector<Sprite*> GetColliders() const					{ return { m_pBodyCollidors[0], m_pBodyCollidors[1] }; }
	void UpdateTankMove(TankMove& move);
	bool HasOwner() const										{ return m_TankMove.CharacterUID != INVALID_UID; }
	void SetCharacterUID(int characterUID)						{ m_TankMove.CharacterUID = characterUID; }
	int	 GetCharacterUID() const								{ return m_TankMove.CharacterUID; }

	// UpdateTankMove() 함수가 호출되면 true로됨
	//bool IsUpdated() const										{ return m_bUpdated;}
	TankMove& GetMove()											{ return m_TankMove; }
	void SetFireCallback(JCore::Action<Bullet*> callback)		{ m_FilreCallBack = callback; }
	void Fire();

	bool IsCollide(Bullet* bullet);
private:
	bool m_bMoveable = false;		// 탱크를 키보드로 움직일 수 있는지
	bool m_bFireable = false;		// 탱크를 키보드를 눌러서 총을 쏠 수 있는지

	Layer* m_pActiveLayer = nullptr;
	Sprite* m_pBodyCollidors[2] = { nullptr, nullptr };
	Scale9Sprite* m_pTower = nullptr;
	Scale9Sprite* m_pGun = nullptr;
	Node* m_pFirePos;

	float m_fShotDelay = 0.0f;			// 총 쏠수 있는
	bool m_bShotable = false;			// 딜레이시간이 경과해서 이제 총을 쏠 수 있는지
	JCore::Action<Bullet*> m_FilreCallBack = nullptr; // 총 발사할때마다 호출할 콜백 함수

	TankMove m_TankMove{
		INVALID_UID,
		0.0, 0.0,
		TANK_MOVE_SPEED,
		MoveDirection::None,
		RotateDirection::None,
		0.0f,
		TANK_ROTATION_SPEED };
	JCore::HashMap<EventKeyboard::KeyCode, bool> m_KeyPressedMap;
};

