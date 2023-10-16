/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:30:51 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Scene/BaseScene.h>

class LoginScene : public BaseScene
{
public:
	LoginScene();
	~LoginScene() override;

	CREATE_FUNC(LoginScene);
	bool init() override;

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void update(float delta) override;

	void OnClickedReconnectButton(TextButton* sender);
	void OnClickedLoginButton(TextButton* sender);
	void OnClickedRegisterButton(TextButton* sender);
	Type getType() const override { return Login; }
private:
	cocos2d::ui::EditBox* m_pIdEditBox;
	cocos2d::ui::EditBox* m_pPassEditBox;
};