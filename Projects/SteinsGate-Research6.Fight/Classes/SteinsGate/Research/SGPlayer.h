/*
 * 작성자: 윤정도
 * 생성일: 2/4/2023 9:36:43 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGCharacter.h>

class SGMapLayer;
class SGPlayer
{
public:
	SGPlayer();
	virtual ~SGPlayer();

	void setCharacter(SGCharacter* character);
	void setMapLayer(SGMapLayer* mapLayer);

	SGActorSprite* getActorSprite();
	SGCharacter* getCharacter();
	SGMapLayer* getMapLayer();

	virtual void runAnimation(int animationCode) = 0;
protected:
	SGMapLayer* m_pMapLayer;
	SGCharacter* m_pCharacter;
};