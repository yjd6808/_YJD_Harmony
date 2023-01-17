/*
 * 작성자: 윤정도
 * 생성일: 1/14/2023 7:07:26 AM
 * =====================
 *
 */


#include "PlayerActionSliding.h"

#include <SteinsGate/Research/Player.h>

using namespace cocos2d;


PlayerActionSliding* PlayerActionSliding::createRetain(SpriteDirection_t direction) {
	auto action = new PlayerActionSliding(direction);

	if (action->init()) {
		action->autorelease();
		action->retain();
		return action;
	}

	DebugAssertMessage(false, "초기화에 실패하였습니다.");
	CC_SAFE_DELETE(action);
	return nullptr;
}

bool PlayerActionSliding::init() {
	switch (m_eDirection) {
	case SpriteDirection::Left: {
		initWithKeys({ ControlKey::Left, ControlKey::Left, ControlKey::Attack });
		m_iActionType = ActionType::SlidingLeft;
		break;
	}
	case SpriteDirection::Right: {
		initWithKeys({ ControlKey::Right, ControlKey::Right, ControlKey::Attack });
		m_iActionType = ActionType::SlidingRight;
		break;
	}
	default: return false;
	}

	m_bMoveable = false;
	return true;
}

void PlayerActionSliding::onActionBegin() {
	auto controller = m_pPlayer->getController();


	controller->setCommandable(false);

	EaseCircleActionOut* pEaseOutByDirection = m_eDirection == SpriteDirection::Right ?
		EaseCircleActionOut::create(MoveBy::create(SlidingDuration_v, Vec2{  SlidingMoveByDistance_v, 0.0 })) :
		EaseCircleActionOut::create(MoveBy::create(SlidingDuration_v, Vec2{ -SlidingMoveByDistance_v, 0.0 }));

	m_pPlayer->stopAllActions();
	m_pPlayer->runAction(Sequence::create(
		DelayTime::create(SlidingBeforeDelay_v),
		pEaseOutByDirection,
		CallFunc::create([=] {
			stop();
		}),
		nullptr
	));

	m_pPlayer->getCharacterSprite()->setSlide(m_eDirection);
}
