/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Common/Core/Npk/NpkPackage.h>
#include <SteinsGate/Research/Enums.h>

class TestLayer : public cocos2d::Layer
{
public:
	bool init() override;
	CREATE_FUNC(TestLayer);

	void update(float delta) override;
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
	cocos2d::Sprite* CreateSprite(int imgIndex, int frameIndex);
private:
	JCore::Vector<Node*> Nodes;
};


