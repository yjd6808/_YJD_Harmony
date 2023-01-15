/*
 * 작성자 : 윤정도
 */
#pragma once

#include <SteinsGate/Tutturu.h>

class LoginLayer : public cocos2d::Layer
{
public:
	bool init() override;
	CREATE_FUNC(LoginLayer);

	void update(float delta) override;
	void onKeyPressed2(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased2(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
};


