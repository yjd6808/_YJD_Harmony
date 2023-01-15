/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:01:39 AM
 * =====================
 * - 애니메이션은 프레임덩어리이고 액션이 아님
 * - 애니메이트는 프레임마다 얼마나 재생시킬지 결정하고 Sprite에 적용할 수 있는 액션이다.
 * - 애니메이트는 각 프레임마다 재생시간을 결정할 수가 없고 프레임마다 고정적으로 정할 수 있는 것으로 보임
 * - 따라서 각 프레임마다 보여줄 애니메이션을 시간 단위로 세부적으로 제어할려면 애니메이션을 업데이트에서 수동으로 조작하는 방법이 최선으로 보인다.
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/CharacterPartSpriteFrame.h>

class CharacterPartSprite;
class CharacterPartAnimate : public cocos2d::Animate
{
public:
	static CharacterPartAnimate* create(cocos2d::Animation* animation, CharacterPartSprite* parent, int motionState);

	CharacterPartAnimate(CharacterPartSprite* parent, int motionState)
		: cocos2d::Animate()
		, m_pParent(parent)
		, m_iMotionState(motionState) {}
	~CharacterPartAnimate() override;
public:
	bool init();
	void onFrameBegin();
	void onAnimateEnd() override;
	

	void setDelayPerUnit(float unit);
	int getMotionState() { return m_iMotionState; }
	int getFrameCount() { return _animation->getFrames().size(); }
	CharacterPartSpriteFrame* getFrameAt(int idx);
	CharacterPartSpriteFrame* getLastFrame();
	CharacterPartSpriteFrame* getFirstFrame();
	int getPartType();
private:
	CharacterPartSprite* m_pParent;
	int m_iMotionState;
};


