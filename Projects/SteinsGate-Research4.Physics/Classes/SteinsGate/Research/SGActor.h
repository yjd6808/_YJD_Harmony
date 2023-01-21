/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:50:32 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGStruct.h>

class SGActor : public cocos2d::Node
{
public:
	SGActor(ActorType_t type, int code);

	void initThicknessBox(const SGThicknessBox& thicknessBox);

	SGThicknessBox getThicknessBox();
	cocos2d::Rect getThicknessBoxRect();
	cocos2d::Vec2 getPositionReal();			// 두께박스 좌하단의 위치
	cocos2d::Vec2 getPositionRealCenter();		// 두께박스 중앙의 위치

	void setPositionReal(float x, float y);
	void setPositionRealCenter(float x, float y);

protected:
	ActorType_t m_eActorType;
	int m_iCode;

	cocos2d::DrawNode* m_pThicknessBox;
};



