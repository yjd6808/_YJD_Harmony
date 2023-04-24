/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:48:30 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/Player.h>
#include <SteinsGate/Client/PlayerController.h>
#include <SteinsGate/Client/ActionMgr.h>


class HostPlayer : public Player
{
	HostPlayer();
public:
	~HostPlayer() override;

	static HostPlayer* get() {
		static HostPlayer* s_pInstance = nullptr;

		if (s_pInstance == nullptr) {
			s_pInstance = dbg_new HostPlayer;
		}
		return s_pInstance;
	}

	bool init();
	void initActionManager();
	void initController();
	void hit(const SGHitInfo& hitInfo) override;
	
	void update(float dt) override;
	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) override;

	void playAction(int actionCode);
	void playActionForce(int actionCode);
	void playBaseActionForce(BaseAction_t baseActionType);
	void playBaseAction(BaseAction_t baseAction);

	int getRunningActionCode();

	ActionMgr* actionManager();
	PlayerController* ctrl();
private:
	AccountData m_AccountData;

	ActionMgr* m_pActionManager;
	PlayerController* m_pController;
};


