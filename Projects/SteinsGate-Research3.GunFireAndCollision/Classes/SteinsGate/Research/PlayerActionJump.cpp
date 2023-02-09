/*
 * 작성자: 윤정도
 * 생성일: 1/17/2023 8:42:20 AM
 * =====================
 *
 */


#include "PlayerActionJump.h"

#include <SteinsGate/Research/Player.h>

using namespace cocos2d;

PlayerActionJump* PlayerActionJump::createRetain() {
	auto action = new PlayerActionJump();

	if (action->init()) {
		action->autorelease();
		action->retain();
		return action;
	}

	DebugAssertMessage(false, "초기화에 실패하였습니다.");
	CC_SAFE_DELETE(action);
	return nullptr;
}


bool PlayerActionJump::init() {
	m_bMoveable = true;
	m_bCancelable = false;

	initWithKeys({ ControlKey::Jump });

	// 총알 쏠 수 있는 횟수 필요
	m_fJumpVelocity = 0.0f;
	m_fJumpVelocityMax = 420.0f;
	m_fJumpGravity = 320.0f;
	return true;
}

void PlayerActionJump::onActionBegin() {
	auto pPreviousAction = m_pPlayer->getPlayerActionManager()->getPreviousAction();
	if (pPreviousAction && (pPreviousAction->getActionType() == ActionType::RunRight || pPreviousAction->getActionType() == ActionType::RunLeft)) {
		m_fMoveSpeedFPSX = DefaultRunSpeedPerFPSX_v;
	}

	auto pCharacter = m_pPlayer->getCharacterSprite();
	m_bMoveable = false;

	m_pPlayer->stopAllActions();
	pCharacter->runAction(Sequence::create(
		cocos2d::CallFunc::create([=] { pCharacter->runMotion(MotionState::JumpBegin); }),
		DelayTime::create(0.1f),
		cocos2d::CallFunc::create([=] { this->m_bMoveable = true; }),
		Spawn::create(
			Sequence::create(
				JumpBy::create(2.0f, {}, 350, 1), 
				CallFunc::create([this]() {stop(); }), nullptr
			),
			CallFunc::create([=] {
				pCharacter->runMotionRepeat(MotionState::JumpingUp);
			}),
			nullptr
		),
		nullptr
	));
}

void PlayerActionJump::onKeyPressed(PlayerController* controller, cocos2d::EventKeyboard::KeyCode keyCode) {

}

