/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 3:38:18 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGFrameTexture.h>

class SGActorPartSprite;
class SGActorPartAnimation : public cocos2d::Ref
{
public:
	SGActorPartAnimation* create();

	void update(float dt);
	void run();

	int getRunningFrameIndex() { return m_iRunningFrameIndex; }
private:
	float m_fRunningFrameTime;
	float m_fLimitFrameTime;
	int m_iRunningFrameIndex;

	bool m_bRepeat;
	bool m_bStart;


	SGActorPartSprite* m_pTarget;
	SGVector<SGFrameTexture*> m_Frames;
};


