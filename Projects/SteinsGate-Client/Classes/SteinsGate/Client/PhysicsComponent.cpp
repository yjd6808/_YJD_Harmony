/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:06:01 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "PhysicsComponent.h"

#include <SteinsGate/Common/RectPoly.h>

#include <SteinsGate/Client/MapLayer.h>

USING_NS_JC;
USING_NS_CC;

//////////////////////////////////////////////////////////////////////////////////////////
PhysicsComponent::PhysicsComponent(Actor* _pActor)
: ActorComponent(_pActor)
, useElasticity_(false)
, bounced_(false)
, weight_(0.0f)
, upTime_(0.0f)
, downTime_(0.0f)
, elapsedPausedTime_(0.0f)
, pauseTime_(0.0f)
, atkBoxInstantElapsedTime_(0.0f)
, atkThicknessBox_(nullptr)
, atkHitBox_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::initialize()
{
	useElasticity_ = false;
	bounced_ = false;
	upTime_ = 0.0f;
	downTime_ = 0.0f;
	velocity_.x = 0.0f;
	velocity_.y = 0.0f;
	pauseTime_ = 0.0f;
	elapsedPausedTime_ = 0.0f;
	atkBoxInstantElapsedTime_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PhysicsComponent::isPaused()
{
	return elapsedPausedTime_ < pauseTime_;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PhysicsComponent::isBounced()
{
	return bounced_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::disableElasticity()
{
	useElasticity_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::enableElasticity()
{
	useElasticity_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::onUpdate(float _dt)
{
	updatePauseTime(_dt);
	updatePhysics(_dt);
	updateDebug(_dt); // TODO: 디버깅용 임시 코드
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::updatePauseTime(float _dt)
{
	elapsedPausedTime_ += _dt;

	if (elapsedPausedTime_ >= pauseTime_)
	{
		pauseTime_ = 0.0f;
		elapsedPausedTime_ = 0.0f;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::updatePhysics(float _dt)
{
	if (isPaused())
	{
		return;
	}

	updateGravity(_dt);
	updateFriction(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::updateGravity(float _dt)
{
	const MapLayer* pMapLayer = pActor_->getMapLayer();

	if (pMapLayer == nullptr)
	{
		return;
	}

	const MapPhysicsInfo* pPhysicsInfo = pMapLayer->getMapPhysicsInfo();

	if (pPhysicsInfo == nullptr)
	{
		return;
	}

	ActorSprite* pActorSprite = pActor_->getActorSprite();

	if (pActorSprite == nullptr)
	{
		return;
	}

	float y = pActorSprite->getPositionY() + velocity_.y * _dt;
	velocity_.y -= pPhysicsInfo->gravity_ * _dt;

	if (y > SG_FLT_EPSILON && velocity_.y > 0.0f)
	{
		upTime_ += _dt;
		downTime_ = 0.0f;
	}

	if (y > SG_FLT_EPSILON && velocity_.y < 0.0f)
	{
		upTime_ = 0.0f;
		downTime_ += _dt;
	}

	if (y <= SG_FLT_EPSILON)
	{
		if (useElasticity_ && !bounced_ && downTime_ > 0.0f)
		{
			velocity_.y = Math::Abs(velocity_.y / pPhysicsInfo->elasticityDividedForce_);
			bounced_ = true;
		}
		else
		{
			velocity_.y = 0.0f;
			bounced_ = false;
		}

		downTime_ = 0.0f;
		y = 0.0f;
	}

	pActorSprite->setPositionY(y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::updateFriction(float _dt)
{
	const MapLayer* pMapLayer = pActor_->getMapLayer();

	if (pMapLayer == nullptr)
	{
		return;
	}

	const MapPhysicsInfo* pPhysicsInfo = pMapLayer->getMapPhysicsInfo();
	const MapAreaInfo* pAreaInfo = pMapLayer->getMapAreaInfo();

	if (pPhysicsInfo == nullptr || pAreaInfo == nullptr)
	{
		return;
	}

	SGRect groundRect = pActor_->getThicknessBoxRect();
	groundRect.origin.x += velocity_.x * _dt;

	if (pActor_->isOnTheGround())
	{
		if (velocity_.x > 0.0f)
		{
			velocity_.x -= pPhysicsInfo->friction_ * _dt;

			if (velocity_.x <= 0.0f)
			{
				velocity_.x = 0.0f;
			}
		}
		else if (velocity_.x < 0.0f)
		{
			velocity_.x += pPhysicsInfo->friction_ * _dt;

			if (velocity_.x >= 0.0f)
			{
				velocity_.x = 0.0f;
			}
		}
	}

	if (pAreaInfo->CheckWall(groundRect.origin.x, groundRect.origin.y))
	{
		return;
	}

	if (pMapLayer->isCollideWithMapObjects(groundRect))
	{
		return;
	}

	pActor_->setPositionRealX(groundRect.origin.x);
}

//////////////////////////////////////////////////////////////////////////////////////////
float PhysicsComponent::addForceX(float _force)
{
	float before = velocity_.x;
	velocity_.x += _force;
	return before;
}

//////////////////////////////////////////////////////////////////////////////////////////
float PhysicsComponent::addForceY(float _force)
{
	float before = velocity_.y;
	velocity_.y += _force;
	return before;
}

//////////////////////////////////////////////////////////////////////////////////////////
float PhysicsComponent::removeForceX()
{
	float before = velocity_.x;
	velocity_.x = 0.0f;
	return before;
}

//////////////////////////////////////////////////////////////////////////////////////////
float PhysicsComponent::removeForceY()
{
	float before = velocity_.y;
	velocity_.y = 0.0f;
	return before;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PhysicsComponent::hasForceX()
{
	return Math::Abs(velocity_.x) >= SG_FLT_EPSILON;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PhysicsComponent::hasForceY()
{
	return Math::Abs(velocity_.y) >= SG_FLT_EPSILON;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::hit(const HitInfo& _hitInfo)
{
	const Actor* pAttacker = _hitInfo.Attacker;
	const SpriteDirection_t hitDirection = _hitInfo.HitDirection;
	const SGRect& hitRect = _hitInfo.HitRect;
	const AttackDataInfo* pAttackDataInfo = _hitInfo.AttackDataInfo;

	float forceX = 500.0f;

	switch (pAttackDataInfo->attackXForceDir_)
	{
	case AttackXForceDirection::Forward:
		forceX = pAttacker->getSpriteDirection() == SpriteDirection::Right
			         ? pAttackDataInfo->attackXForce_
			         : -pAttackDataInfo->attackXForce_;
		break;

	case AttackXForceDirection::Backward:
		forceX = pAttacker->getSpriteDirection() == SpriteDirection::Right
			         ? -pAttackDataInfo->attackXForce_
			         : pAttackDataInfo->attackXForce_;
		break;

	case AttackXForceDirection::Spread:
	case AttackXForceDirection::None:
		forceX = hitDirection == SpriteDirection::Right
			         ? -pAttackDataInfo->attackXForce_
			         : pAttackDataInfo->attackXForce_;
		break;

	default:
		DebugAssertMsg(false, "뭐야! 이상한 타입의 X 포스 디렉션입니다.");
		break;
	}

	bounced_ = false;
	pActor_->setSpriteDirection(hitDirection);

	float removedForceX = removeForceX();
	float removedForceY = removeForceY();

	if (pAttackDataInfo->isFallDownAttack_)
	{
		addForceX(forceX);
		addForceY(pAttackDataInfo->attackYForce_);
		return;
	}

	if (removedForceX != 0.0f)
	{
		addForceY(pAttackDataInfo->attackYForce_);
		return;
	}

	if (pActor_->isOnTheGround())
	{
		addForceX(forceX);
		return;
	}

	addForceY(pAttackDataInfo->attackYForce_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::pausePhysics()
{
	pausePhysics(FLT_MAX);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::pausePhysics(float _time)
{
	pauseTime_ = _time;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::stiffenBody(float _time)
{
	pausePhysics(_time);
	pActor_->pauseAnimation(_time);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::resume()
{
	pauseTime_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
Direction_t PhysicsComponent::getForceXDirection()
{
	if (velocity_.x > SG_FLT_EPSILON)
	{
		return Direction::Right;
	}

	if (velocity_.x < -SG_FLT_EPSILON)
	{
		return Direction::Left;
	}

	return Direction::None;
}

//////////////////////////////////////////////////////////////////////////////////////////
Direction_t PhysicsComponent::getForceYDirection()
{
	if (velocity_.y > SG_FLT_EPSILON)
	{
		return Direction::Up;
	}

	if (velocity_.y < -SG_FLT_EPSILON)
	{
		return Direction::Down;
	}

	return Direction::None;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PhysicsComponent::hasForce()
{
	if (Math::Abs(velocity_.x) >= SG_FLT_EPSILON)
	{
		return true;
	}

	if (Math::Abs(velocity_.y) >= SG_FLT_EPSILON)
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::updateDebug(float _dt)
{
	if (!Global::Get()->DrawAttackBox)
	{
		if (atkThicknessBox_)
		{
			atkThicknessBox_->setOpacity(0);
		}

		if (atkHitBox_)
		{
			atkHitBox_->setOpacity(0);
		}

		return;
	}

	updateDebugSub1(_dt);
	updateDebugSub2(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::updateDebugSub1(float _dt)
{
	MapLayer* pMapLayer = pActor_->getMapLayer();
	const ActorSprite* pActorSprite = pActor_->getActorSprite();

	if (pMapLayer == nullptr)
	{
		return;
	}

	ActorPartAnimation* pAnimation = pActorSprite->getRunningAnimation();

	if (pAnimation == nullptr)
	{
		return;
	}

	const FrameInfo& frameInfo = pAnimation->getRunningFrameInfo();

	if (frameInfo.FrameEventCode == InvalidValue_v)
	{
		return;
	}

	const FrameEvent* pFrameEvent = Core::DataManager->getFrameEvent(pActor_->getType(), frameInfo.FrameEventCode);

	if (pFrameEvent && pFrameEvent->type_ != FrameEventType::AttackBoxInstant)
	{
		return;
	}

	// 어택박스 프레임 발견할때마다 초기화
	atkBoxInstantElapsedTime_ = 0.0f;

	FrameEventAttackBoxInstant* pAttackBoxInstantInfo = (FrameEventAttackBoxInstant*)pFrameEvent;
	ActorRect absoluteActorRect = Actor::convertAbsoluteActorRect(pActor_, pAttackBoxInstantInfo->rect_);

	RectPoly hitPoly = RectPoly::createFromLeftBottom(
		absoluteActorRect.BodyRect.origin,
		absoluteActorRect.BodyRect.size);

	RectPoly thickPoly = RectPoly::createFromLeftBottom(
		absoluteActorRect.ThicknessRect.origin,
		absoluteActorRect.ThicknessRect.size);

	if (atkHitBox_ == nullptr)
	{
		atkHitBox_ = DrawNode::create();
		atkHitBox_->drawPolygon(hitPoly.source(), 4, {}, 1.0f, Color4F{ Color3B::YELLOW, 0.7f });
		pMapLayer->addChild(atkHitBox_, 1000);
	}
	else
	{
		atkHitBox_->clear();
		atkHitBox_->setOpacity(255);
		atkHitBox_->drawPolygon(hitPoly.source(), 4, {}, 1.0f, Color4F{ Color3B::YELLOW, 0.7f });
	}

	if (atkThicknessBox_ == nullptr)
	{
		atkThicknessBox_ = DrawNode::create();
		atkThicknessBox_->drawPolygon(thickPoly.source(), 4, {}, 1.0f, Color4F{ Color3B::YELLOW, 0.7f });
		pMapLayer->addChild(atkThicknessBox_, 1000);
	}
	else
	{
		atkThicknessBox_->clear();
		atkThicknessBox_->setOpacity(255);
		atkThicknessBox_->drawPolygon(thickPoly.source(), 4, {}, 1.0f, Color4F{ Color3B::YELLOW, 0.7f });
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::updateDebugSub2(float _dt)
{
	atkBoxInstantElapsedTime_ += _dt;

	if (atkBoxInstantElapsedTime_ >= 1.0f)
	{
		if (atkThicknessBox_)
		{
			atkThicknessBox_->setOpacity(0);
		}

		if (atkHitBox_)
		{
			atkHitBox_->setOpacity(0);
		}
	}
}
