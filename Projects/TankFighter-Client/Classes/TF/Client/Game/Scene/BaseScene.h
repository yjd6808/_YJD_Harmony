/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:57 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Layer/GridLayer.h>

#include <cocos/ui/CocosGUI.h>

class BaseScene : public cocos2d::Scene
{
public:
	BaseScene();
	
	bool init() override;

	void update(float delta) override;

private:
	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
protected:
	cocos2d::Layer* m_pUILayer;
	GridLayer* m_pGridLayer;
	cocos2d::ui::Text* m_pNetInfo;
};