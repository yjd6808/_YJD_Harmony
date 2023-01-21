/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:11 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGStruct.h>
#include <SteinsGate/Research/SGActorPartAnimation.h>

class SGActorPartSprite : public cocos2d::Sprite
{
public:
	SGActorPartSprite() {}

	bool init() override;

private:

	SGHashMap<int, SGActorPartAnimation*> m_AnimationMap;
};


