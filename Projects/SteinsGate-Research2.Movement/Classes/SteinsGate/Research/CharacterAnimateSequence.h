/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 11:38:29 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/CharacterPartAnimate.h>

class CharacterAnimateSequence : cocos2d::Sequence
{
public:
	//static CharacterAnimateSequence* create()
private:
	JCore::Vector<cocos2d::FiniteTimeAction*> m_vActions;
};
