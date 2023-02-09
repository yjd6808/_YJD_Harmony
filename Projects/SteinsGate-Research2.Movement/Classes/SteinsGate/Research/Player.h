/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 4:10:29 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Config.h>

#include <SteinsGate/Common/Engine.h>
#include <SteinsGate/Research/PlayerController.h>
#include <SteinsGate/Research/CharacterSprite.h>

#include <JCore/Time.h>

class Player : public cocos2d::Node
{
public:
	static Player* create();

	bool init() override;

	void update(float dt);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onFrameBegin(int frameIndex, int frameIndexInAnimation);
private:
	CharacterSprite* m_pCharacterSprite;
	PlayerController* m_pController;
};