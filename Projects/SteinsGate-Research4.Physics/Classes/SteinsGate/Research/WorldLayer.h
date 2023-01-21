/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Common/Core/Npk/NpkPackage.h>
#include <SteinsGate/Research/Enums.h>
#include <SteinsGate/Research/SGPlayer.h>

class WorldLayer : public cocos2d::Layer
{
public:
	~WorldLayer() override;

	bool init() override;
	static WorldLayer* create();

	void update(float dt) override;
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
public:
	SGPlayer* m_pPlayer;
};


