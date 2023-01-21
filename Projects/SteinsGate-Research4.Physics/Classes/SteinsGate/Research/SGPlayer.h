/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:48:30 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGCharacter.h>

class SGPlayer
{
public:
	SGPlayer();
	~SGPlayer();

	void update(float dt);
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);


	void setCharacter(SGCharacter* character);
	SGCharacter* getCharacter();
private:
	SGCharacter* m_pCharacter;

};


