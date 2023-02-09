/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:48:30 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGPlayer.h>
#include <SteinsGate/Research/SGPlayerController.h>
#include <SteinsGate/Research/SGActionManager.h>


class SGHostPlayer : public SGPlayer
{
	SGHostPlayer();
public:
	~SGHostPlayer();

	static SGHostPlayer* get() {
		static SGHostPlayer* s_pInstance = nullptr;

		if (s_pInstance == nullptr)
			s_pInstance = new SGHostPlayer;

		return s_pInstance;
	}

	
	void initActionManager();
	void initController();

	void update(float dt);
	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);

	void runAnimation(int animationCode) override;
	void runAction(int actionCode);
	void runActionForce(int actionCode);
	void runBaseAction(BaseAction_t baseAction);
	

	int getRunningActionCode();

	SGActionManager* getActionManager();
	SGPlayerController* getController();
private:

	SGActionManager* m_pActionManager;
	SGPlayerController* m_pController;
};


