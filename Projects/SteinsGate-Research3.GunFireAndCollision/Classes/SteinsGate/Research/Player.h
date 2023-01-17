/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:29 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Collider.h>
#include <SteinsGate/Research/PlayerActionManager.h>
#include <SteinsGate/Research/PlayerController.h>
#include <SteinsGate/Research/CharacterSprite.h>

class Player : public Collider
{
public:
	static Player* create(int type, int id);

	Player(int type, int id);
	~Player() override;

	bool init() override;

	void update(float dt);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	CharacterSprite* getCharacterSprite() { return m_pCharacterSprite; }
	PlayerActionManager* getPlayerActionManager() { return m_pActionManager; }
	PlayerController* getController() { return m_pController; }
	SpriteDirection_t getSpriteDirection();

	cocos2d::Rect getThicknessBox() override;
	cocos2d::Rect getHitBox() override;
	void setRealPos(float x, float y) override;
	cocos2d::Vec2 getRealPos() override;
	cocos2d::Vec2 getRealPosCenter() override;

	cocos2d::Vec2 getTargetCanvasPosition() override;

private:
	CharacterSprite* m_pCharacterSprite{};
	PlayerActionManager* m_pActionManager{};
	PlayerController* m_pController{};

};