#pragma once

#include <TF/SourceHeader.h>
#include <TF/Object/Tank.h>
#include <TF/Object/Obstacle.h>
#include <TF/UI/TextButton.h>

class LoginLayer : public Layer
{
public:
	bool init() override;
	CREATE_FUNC(LoginLayer);

	void update(float delta) override;
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) override;
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;

	void OnClickedReconnectButton(TextButton* sender);
	void OnClickedLoginButton(TextButton* sender);
	void OnClickedRegisterButton(TextButton* sender);

	void CmdLoginAck(ICommand* cmd);
	void CmdRegisterAck(ICommand* cmd);
private:
	Tank* m_pTank = nullptr;
	Obstacle* m_pObstacle = nullptr;
};


