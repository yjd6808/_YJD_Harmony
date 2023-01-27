/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:48:30 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGCharacter.h>
#include <SteinsGate/Research/SGPlayerController.h>
#include <SteinsGate/Research/SGActionManager.h>

class SGMapLayer;
class SGMonster;
class SGProjectile;
class SGPlayer
{
public:
	SGPlayer();
	~SGPlayer();

	void setCharacter(SGCharacter* character);
	void setMapLayer(SGMapLayer* mapLayer);
	void initActionManager();
	void initController();
	void update(float dt);
	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);

	void runAction(int actionCode);
	void runBaseAction(BaseAction_t baseAction);
	void runAnimation(int animationCode);
	
	SGCharacter* getCharacter();
	SGActionManager* getActionManager();
	SGPlayerController* getController();
	SGMapLayer* getMapLayer();
private:
	SGMapLayer* m_pMapLayer;
	SGCharacter* m_pCharacter;
	SGActionManager* m_pActionManager;
	SGPlayerController* m_pController;
};


