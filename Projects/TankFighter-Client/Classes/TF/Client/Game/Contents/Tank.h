/*
 * 작성자: 윤정도
 * 생성일: 10/20/2023 5:52:31 PM
 * =====================
 *
 */


#pragma once

#include <cocos/ui/CocosGUI.h>

#include <TF/Client/Game/Contents/Bullet.h>

class Tank : public cocos2d::ui::Scale9Sprite
{
public:
	Tank();

	CREATE_FUNC(Tank)

	bool init() override;

	void update(float delta) override;
	void updateRotation(float delta);
	void updatePosition(float delta);
	void updateShotFire(float delta);

	void fire();

	void spawn(int characterPrimaryKey, const TankMove& initialMove);
	void free();

	void setFireCallback(JCore::Action<Tank*> callback) { m_FilreCallBack = callback; }
	void setTankMove(const TankMove& move);
	void setMoveable(bool moveable) { m_bMoveable = moveable; }
	void setFireable(bool fireable) { m_bFireable = fireable; }
	void setDeath(bool death);
	void sendMove();

	void getMove(JCORE_REF_OUT TankMove& move);
	int getCharacterPrimaryKey() const;
	cocos2d::Vec2 getFirePosition() const;

	void updateRotateDirection();
	void updateMoveDirection();

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	bool isCollide(Bullet* bullet);
	bool isDeath();
private:
	int m_iCharacterPrimaryKey;

	Sprite* m_pBodyCollidors[2];
	Sprite* m_pTower;
	Sprite* m_pGun;
	Node* m_pFirePosition;

	bool m_bMoveable;							// 탱크를 키보드로 움직일 수 있는지
	bool m_bFireable;							// 탱크를 키보드를 눌러서 총을 쏠 수 있는지

	TankMove m_Move;

	JCore::TimeCounterF m_fShotTimeCounter;
	JCore::Action<Tank*> m_FilreCallBack;		// 총 발사할때마다 호출할 콜백 함수
	JCore::HashMap<cocos2d::EventKeyboard::KeyCode, bool> m_KeyPressedMap;
};
