/*
 * 작성자: 윤정도
 * 생성일: 8/9/2023 10:06:01 AM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "PhysicsComponent.h"

#include <sge/RectPoly.h>

#include <sgcl/MapLayer.h>
#include <sg/_Struct/SteinsGate_AttackData.h>

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
void PhysicsComponent::Initialize()
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
bool PhysicsComponent::IsPaused()
{
	return elapsedPausedTime_ < pauseTime_;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PhysicsComponent::IsBounced()
{
	return bounced_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::DisableElasticity()
{
	useElasticity_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::EnableElasticity()
{
	useElasticity_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::OnUpdate(float _dt)
{
	UpdatePauseTime(_dt);
	UpdatePhysics(_dt);
	UpdateDebug(_dt); // TODO: 디버깅용 임시 코드
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::UpdatePauseTime(float _dt)
{
	elapsedPausedTime_ += _dt;

	if (elapsedPausedTime_ >= pauseTime_)
	{
		pauseTime_ = 0.0f;
		elapsedPausedTime_ = 0.0f;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::UpdatePhysics(float _dt)
{
	if (IsPaused())
	{
		return;
	}

	UpdateGravity(_dt);
	UpdateFriction(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::UpdateGravity(float _dt)
{
	const MapLayer* pMapLayer = pActor_->GetMapLayer();

	if (pMapLayer == nullptr)
	{
		return;
	}

	const MapPhysicsInfo* pPhysicsInfo = pMapLayer->GetMapPhysicsInfo();

	if (pPhysicsInfo == nullptr)
	{
		return;
	}

	ActorSprite* pActorSprite = pActor_->GetActorSprite();

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
void PhysicsComponent::UpdateFriction(float _dt)
{
	const MapLayer* pMapLayer = pActor_->GetMapLayer();

	if (pMapLayer == nullptr)
	{
		return;
	}

	const MapPhysicsInfo* pPhysicsInfo = pMapLayer->GetMapPhysicsInfo();
	const MapAreaInfo* pAreaInfo = pMapLayer->GetMapAreaInfo();

	if (pPhysicsInfo == nullptr || pAreaInfo == nullptr)
	{
		return;
	}

	cc::rect groundRect = pActor_->GetThicknessBoxRect();
	groundRect.origin.x += velocity_.x * _dt;

	if (pActor_->IsOnTheGround())
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

	if (pMapLayer->IsCollideWithMapObjects(groundRect))
	{
		return;
	}

	pActor_->SetPositionRealX(groundRect.origin.x);
}

//////////////////////////////////////////////////////////////////////////////////////////
float PhysicsComponent::AddForceX(float _force)
{
	float before = velocity_.x;
	velocity_.x += _force;
	return before;
}

//////////////////////////////////////////////////////////////////////////////////////////
float PhysicsComponent::AddForceY(float _force)
{
	float before = velocity_.y;
	velocity_.y += _force;
	return before;
}

//////////////////////////////////////////////////////////////////////////////////////////
float PhysicsComponent::RemoveForceX()
{
	float before = velocity_.x;
	velocity_.x = 0.0f;
	return before;
}

//////////////////////////////////////////////////////////////////////////////////////////
float PhysicsComponent::RemoveForceY()
{
	float before = velocity_.y;
	velocity_.y = 0.0f;
	return before;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PhysicsComponent::HasForceX()
{
	return Math::Abs(velocity_.x) >= SG_FLT_EPSILON;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PhysicsComponent::HasForceY()
{
	return Math::Abs(velocity_.y) >= SG_FLT_EPSILON;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::Hit(const HitInfo& _hitInfo)
{
	const Actor* pAttacker = _hitInfo.pAttacker_;
	const SpriteDirection_t hitDirection = _hitInfo.hitDirection_;
	const cc::rect& hitRect = _hitInfo.hitRect_;
	const AttackDataInfo* pAttackDataInfo = _hitInfo.pAttackDataInfo_;

	float forceX = 500.0f;

	switch (pAttackDataInfo->attackXForceDir_)
	{
	case AttackXForceDirection::Forward:
		forceX = pAttacker->GetSpriteDirection() == SpriteDirection::Right
			         ? pAttackDataInfo->attackXForce_
			         : -pAttackDataInfo->attackXForce_;
		break;

	case AttackXForceDirection::Backward:
		forceX = pAttacker->GetSpriteDirection() == SpriteDirection::Right
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
		jc_assert_msg(false, "뭐야! 이상한 타입의 X 포스 디렉션입니다.");
		break;
	}

	bounced_ = false;
	pActor_->SetSpriteDirection(hitDirection);

	float removedForceX = RemoveForceX();
	float removedForceY = RemoveForceY();

	if (pAttackDataInfo->isFallDownAttack_)
	{
		AddForceX(forceX);
		AddForceY(pAttackDataInfo->attackYForce_);
		return;
	}

	if (removedForceX != 0.0f)
	{
		AddForceY(pAttackDataInfo->attackYForce_);
		return;
	}

	if (pActor_->IsOnTheGround())
	{
		AddForceX(forceX);
		return;
	}

	AddForceY(pAttackDataInfo->attackYForce_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::PausePhysics()
{
	PausePhysics(FLT_MAX);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::PausePhysics(float _time)
{
	pauseTime_ = _time;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::StiffenBody(float _time)
{
	PausePhysics(_time);
	pActor_->PauseAnimation(_time);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::Resume()
{
	pauseTime_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////
Direction_t PhysicsComponent::GetForceXDirection()
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
Direction_t PhysicsComponent::GetForceYDirection()
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
bool PhysicsComponent::HasForce()
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
void PhysicsComponent::UpdateDebug(float _dt)
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

	UpdateDebugSub1(_dt);
	UpdateDebugSub2(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PhysicsComponent::UpdateDebugSub1(float _dt)
{
	MapLayer* pMapLayer = pActor_->GetMapLayer();
	const ActorSprite* pActorSprite = pActor_->GetActorSprite();

	if (pMapLayer == nullptr)
	{
		return;
	}

	ActorPartAnimation* pAnimation = pActorSprite->GetRunningAnimation();

	if (pAnimation == nullptr)
	{
		return;
	}

	const FrameInfo& frameInfo = pAnimation->GetRunningFrameInfo();

	if (frameInfo.frameEventCode_ == InvalidValue_v)
	{
		return;
	}

	const FrameEvent* pFrameEvent = g_cDescMgr.GetFrameEvent(pActor_->GetType(), frameInfo.frameEventCode_);

	if (pFrameEvent && pFrameEvent->type_ != FrameEventType::AttackBoxInstant)
	{
		return;
	}

	// 어택박스 프레임 발견할때마다 초기화
	atkBoxInstantElapsedTime_ = 0.0f;

	FrameEventAttackBoxInstant* pAttackBoxInstantInfo = (FrameEventAttackBoxInstant*)pFrameEvent;
	ActorRect absoluteActorRect = Actor::ConvertAbsoluteActorRect(pActor_, pAttackBoxInstantInfo->rect_);

	RectPoly hitPoly = RectPoly::createFromLeftBottom(
		absoluteActorRect.bodyRect_.origin,
		absoluteActorRect.bodyRect_.size);

	RectPoly thickPoly = RectPoly::createFromLeftBottom(
		absoluteActorRect.thicknessRect_.origin,
		absoluteActorRect.thicknessRect_.size);

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
void PhysicsComponent::UpdateDebugSub2(float _dt)
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
