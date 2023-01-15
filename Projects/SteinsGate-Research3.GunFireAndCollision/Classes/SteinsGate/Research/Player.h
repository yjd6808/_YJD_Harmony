/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:29 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Research/PlayerActionManager.h>
#include <SteinsGate/Research/PlayerController.h>
#include <SteinsGate/Research/CharacterSprite.h>

class Player : public cocos2d::Node
{
public:
	static Player* create();
	~Player() override;

	bool init() override;

	void update(float dt);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	CharacterSprite* getCharacterSprite() { return m_pCharacterSprite; }
	PlayerActionManager* getPlayerActionManager() { return m_pActionManager; }
	PlayerController* getController() { return m_pController; }

	cocos2d::Rect getThicknessBox();
	cocos2d::Rect getHitBox();

	// 두께박스가 진짜 플레이어 위치 기준이다.
	void setRealPos(const cocos2d::Vec2& v) { setRealPos(v.x, v.y); }
	void setRealPos(float x, float y);
	cocos2d::Vec2 getRealPos();
	float getRealPosX() { return getRealPos().x; }
	float getRealPosY() { return getRealPos().y; }

private:
	CharacterSprite* m_pCharacterSprite{};
	PlayerActionManager* m_pActionManager{};
	PlayerController* m_pController{};
	cocos2d::DrawNode* m_pThicknessBox;

};