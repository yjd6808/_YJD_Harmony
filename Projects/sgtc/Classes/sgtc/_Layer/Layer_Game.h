/*
 * 작성자 : 윤정도
 */
#pragma once

class GameLayer : public cc::Layer
{
public:
	GameLayer();
	~GameLayer() override;

	static GameLayer* Create();

	bool init() override;
	void update(float _dt) override;

	void onKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
	void onKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) override;
private:
};
