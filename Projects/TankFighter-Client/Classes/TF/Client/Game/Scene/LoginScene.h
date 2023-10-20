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

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
	void update(float delta) override;

	void onClickedReconnectButton(TextButton* sender);
	void onClickedLoginButton(TextButton* sender);
	void onClickedRegisterButton(TextButton* sender);
	Type getType() const override { return Type::Login; }

	void refreshIdPass();
	
private:
	cocos2d::ui::EditBox* m_pIdEditBox;
	cocos2d::ui::EditBox* m_pPassEditBox;
};