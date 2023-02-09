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
CharacterPartAnimate* CharacterPartAnimate::create(Animation* animation, CharacterPartSprite* parent)
{
	CharacterPartAnimate* animate = new (std::nothrow) CharacterPartAnimate(parent);

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

	
	/*
	이 방법을 쓰면 전역적으로 동작하기 땜에 고려해야될게 많아진다.
	그냥 쉽게 엔진 코드를 조금 수정하자
	auto& frames = getAnimation()->getFrames();

	for (int i = 0; i < frames.size(); i++) {
		cocos2d::ValueMap userData;
		userData.insert(std::make_pair("FrameIndex", Value(i)));
	}

	auto _listener = EventListenerCustom::create(AnimationFrameDisplayedNotification,
		[this](EventCustom* event) {
			// event->valuemap
		}	
	);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_listener, 1);
	*/
	return true;
}


void CharacterPartAnimate::onFrameBegin() {
	auto pCurrentRunningFrame = dynamic_cast<CharacterPartSpriteFrame*>(getAnimation()
														->getFrames().at(_currFrameIndex)
														->getSpriteFrame());
	DebugAssertMessage(pCurrentRunningFrame, "다른 형식으로 사용할 수 없어요.");
	m_pParent->onFrameBegin(pCurrentRunningFrame);
}

void CharacterPartAnimate::setDelayPerUnit()
{
}

