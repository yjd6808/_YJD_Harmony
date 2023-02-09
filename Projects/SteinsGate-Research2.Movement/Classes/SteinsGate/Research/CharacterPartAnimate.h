/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:01:39 AM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Enums.h>

#include <SteinsGate/Common/Engine.h>
#include <SteinsGate/Research/CharacterPartSpriteFrame.h>

class CharacterPartSprite;
class CharacterPartAnimate : public cocos2d::Animate
{
public:
	static CharacterPartAnimate* create(cocos2d::Animation* animation, CharacterPartSprite* parent);

	CharacterPartAnimate(CharacterPartSprite* parent)
		: cocos2d::Animate()
		, m_pParent(parent) {}
	~CharacterPartAnimate() override;
public:
	bool init();
	void onFrameBegin();
	void setDelayPerUnit();

private:
	CharacterPartSprite* m_pParent;
};