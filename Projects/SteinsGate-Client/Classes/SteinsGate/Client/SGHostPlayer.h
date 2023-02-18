/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:48:30 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/SGPlayer.h>
#include <SteinsGate/Client/SGPlayerController.h>
#include <SteinsGate/Client/SGActionManager.h>


class SGHostPlayer : public SGPlayer
{
	SGHostPlayer();
public:
	~SGHostPlayer() override;

	static SGHostPlayer* get() {
		static SGHostPlayer* s_pInstance = nullptr;

		if (s_pInstance == nullptr) {
			s_pInstance = dbg_new SGHostPlayer;
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
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;

	void playAction(int actionCode);
	void playActionForce(int actionCode);
	void playBaseActionForce(BaseAction_t baseActionType);
	void playBaseAction(BaseAction_t baseAction);

	int getRunningActionCode();

	SGActionManager* actionManager();
	SGPlayerController* ctrl();
private:
	AccountData m_AccountData;

	SGActionManager* m_pActionManager;
	SGPlayerController* m_pController;
};


