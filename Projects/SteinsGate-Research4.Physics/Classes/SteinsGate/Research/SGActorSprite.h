/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:35 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGActorPartSprite.h>

class SGActorSprite : public cocos2d::Sprite
{
public:
	SGActorSprite(const SGActorSpriteDataPtr& actorData);

	static SGActorSprite* create(const SGActorSpriteDataPtr& actorData);

	bool init() override;
	void update(float delta) override;

private:
	SGActorSpriteDataPtr m_spActorData;
	cocos2d::Sprite* m_pCanvas;
	SGVector<SGActorPartSprite*> m_Parts;
};


