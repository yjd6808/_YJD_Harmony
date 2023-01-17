/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Common/Core/Npk/NpkPackage.h>
#include <SteinsGate/Research/Enums.h>
#include <SteinsGate/Research/Player.h>

class WorldLayer : public cocos2d::Layer
{
public:
	bool init() override;
	CREATE_FUNC(WorldLayer);

	void update(float delta) override;
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
	cocos2d::Sprite* CreateSprite(int imgIndex, int frameIndex);
public:
	Player* m_pPlayer;
};


