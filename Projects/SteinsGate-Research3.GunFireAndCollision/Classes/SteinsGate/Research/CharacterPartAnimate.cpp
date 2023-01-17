#include "CharacterPartAnimate.h"

#include "CharacterPartSprite.h"
/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 4:01:53 AM
 * =====================
 *
 */

using namespace cocos2d;

// 오토릴리즈 안함.
CharacterPartAnimate* CharacterPartAnimate::create(Animation* animation, CharacterPartSprite* parent, int motionState)
{
	CharacterPartAnimate* animate = new (std::nothrow) CharacterPartAnimate(parent, motionState);

	if (animate && animate->initWithAnimation(animation) && animate->init()) {
		animate->autorelease();
		return animate;
	}

	CC_SAFE_DELETE(animate);
	return nullptr;
}


CharacterPartAnimate::~CharacterPartAnimate() {
	printf("CharacterPartAnimate 소멸\n");
}

bool CharacterPartAnimate::init() {

	setFrameChangedCallback([this](int frameIndexInAnimation) { this->onFrameBegin(); });
	return true;
}


void CharacterPartAnimate::onFrameBegin() {
	auto pCurrentRunningFrame = getFrameAt(_currFrameIndex);
	DebugAssertMessage(pCurrentRunningFrame, "이상한 타입의 프레임이 있습니다. (1)");
	m_pParent->onFrameBegin(this, pCurrentRunningFrame);
}

void CharacterPartAnimate::onAnimateEnd2() {
	m_pParent->onAnimateEnd(this, getLastFrame());
}

void CharacterPartAnimate::setDelayPerUnit(float unit) {
	
}

CharacterPartSpriteFrame* CharacterPartAnimate::getFrameAt(int idx) {
	auto& frames = _animation->getFrames();
	DebugAssertMessage(idx >= 0 && idx < frames.size(), "프레임내의 인덱스가 아닙니다.");
	auto pCurrentRunningFrame =	dynamic_cast<CharacterPartSpriteFrame*>(frames.at(idx)->getSpriteFrame());
	DebugAssertMessage(pCurrentRunningFrame, "이상한 타입의 프레임이 있습니다. (2)");
	return pCurrentRunningFrame;
}


CharacterPartSpriteFrame* CharacterPartAnimate::getFirstFrame() {
	return getFrameAt(0);
}

int CharacterPartAnimate::getPartType() {
	return m_pParent->getPartType();
}

CharacterPartSpriteFrame* CharacterPartAnimate::getLastFrame() {
	return getFrameAt(getFrameCount() - 1);
}
