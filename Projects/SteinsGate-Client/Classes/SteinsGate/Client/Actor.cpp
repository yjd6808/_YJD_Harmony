/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:07 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "Actor.h"

#include <SteinsGate/Common/RectPoly.h>
#include <SteinsGate/Common/RectEx.h>
#include <SteinsGate/Common/Color.h>

#include <SteinsGate/Client/Global.h>
#include <SteinsGate/Client/MapLayer.h>

#include <SteinsGate/Client/PhysicsComponent.h>
#include <SteinsGate/Client/AIComponent.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
Actor::Actor()
: pMapLayer_(nullptr)
, pActorSprite_(nullptr)
, pHitRecorder_(nullptr)
, actorId_(InvalidValue_v)
, allyFlag_(0)
, cleanUpFlag_(cfNone)
, isCleanUp_(false)
, pThicknessBox_(nullptr)
, pAttacher_(nullptr)
, attches_(4)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Actor::~Actor()
{
	JCORE_DELETE_SAFE(pHitRecorder_);
	listeners_.DeleteAll();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::InitVariables()
{
	// 이거 2개만 초기화 시키면 댈듯?
	actorId_ = InvalidValue_v;
	pMapLayer_ = nullptr;
	attches_.Clear();
	pAttacher_ = nullptr;
	isCleanUp_ = false;
	cleanUpFlag_ = cfNone;

	// m_eActorType = ;
	// m_iCode = ;
	// m_pThicknessBox = nullptr;
	// m_iAllyFlag = 0;
	// m_pHitRecorder = nullptr;
	// m_pActorSprite = nullptr;
	// m_pListener = nullptr;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::AddListener(IActorListener* _pListener)
{
	return listeners_.Add(_pListener);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::HasListener(IActorListener::Type _type)
{
	return listeners_.Has(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
IActorListener* Actor::GetListener(IActorListener::Type _type)
{
	return listeners_.Get(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::InitThicknessBox(const ThicknessBox& _thicknessBox)
{
	// DrawNode는 앵커포인트 신경안쓰고 컨텐츠박스 기준 좌하단부터 그림
	RectPoly poly = RectPoly::createFromLeftBottom(0, 0, _thicknessBox.width_, _thicknessBox.height_);

	if (pThicknessBox_ == nullptr)
	{
		pThicknessBox_ = SGDrawNode::create();
		pThicknessBox_->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		pThicknessBox_->setOpacity(125);
		pThicknessBox_->drawPolygon(poly.source(), 4, {}, 1, SGColor4F{ ColorList::Brightgreen_v });
		this->addChild(pThicknessBox_);
	}
	pThicknessBox_->setPositionX(_thicknessBox.relativeX_);
	pThicknessBox_->setPositionY(_thicknessBox.relativeY_);
	pThicknessBox_->setContentSize(_thicknessBox.width_, _thicknessBox.height_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::InitHitRecorder(int _hitPossibleListSize /* = 16 */, int _alreadyHitMapSize /* = 32 */, Actor* _pOwner /*= nullptr */)
{
	Actor* pOwner = _pOwner;

	if (pOwner == nullptr)
		pOwner = this;

	if (pHitRecorder_ == nullptr)
		pHitRecorder_ = dbg_new HitRecorder(pOwner, this, _hitPossibleListSize, _alreadyHitMapSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::Hit(const HitInfo& _hitInfo)
{
	PhysicsComponent* pPhysicsComponent = GetComponent<PhysicsComponent>();

	if (pPhysicsComponent)
	{
		pPhysicsComponent->Hit(_hitInfo);
	}

	GetListenerCollection().OnHit(_hitInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::update(float _dt)
{
	DebugAssertMsg(isCleanUp_ == false, "다음 프레임에 풀로 복귀 예정중인 객체입니다.");
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	DebugAssertMsg(pMapLayer_, "맵 레이어가 세팅되지 않았습니다.");

	pActorSprite_->update(_dt);
	listeners_.OnUpdate(_dt);
	components_.OnUpdate(_dt);

	if (Core::Contents.Global->DrawThicknessBox)
		pThicknessBox_->setOpacity(0);
	else
		pThicknessBox_->setOpacity(125);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::AddComponent(IComponent* _pComponenet)
{
	components_.Add(_pComponenet);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::HasComponent(IComponent::Type _type) const
{
	return components_.Has(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* Actor::GetTypeName() const
{
	return ActorType::Name[GetType()];
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorRect Actor::GetActorRect() const
{
	return { GetThicknessBoxRect(), GetHitBox() };
}

//////////////////////////////////////////////////////////////////////////////////////////
ThicknessBox Actor::GetThicknessBox() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");

	SGVec2 pos = pThicknessBox_->getPosition();
	SGSize size = pThicknessBox_->getContentSize();

	return { pos.x, pos.y, size.width, size.height };
}

//////////////////////////////////////////////////////////////////////////////////////////
SGDrawNode* Actor::GetThicknessBoxNode() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return pThicknessBox_;
}

//////////////////////////////////////////////////////////////////////////////////////////
Rect Actor::GetThicknessBoxRect() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { GetPositionReal(), pThicknessBox_->getContentSize() };
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 Actor::GetPositionReal() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = GetThicknessBox();
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x += (thicknessBox.relativeX_ - (thicknessBox.width_ / 2.0f));
	thicknessOrigin.y += (thicknessBox.relativeY_ - (thicknessBox.height_ / 2.0f));
	return thicknessOrigin;
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::GetPositionRealX() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = GetThicknessBox();
	float thicknessOriginX = getPositionX();
	thicknessOriginX += (thicknessBox.relativeX_ - (thicknessBox.width_ / 2.0f));
	return thicknessOriginX;
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::GetPositionRealY() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = GetThicknessBox();
	float thicknessOriginY = getPositionY();
	thicknessOriginY += (thicknessBox.relativeY_ - (thicknessBox.height_ / 2.0f));
	return thicknessOriginY;
}

float Actor::GetPositionActorY() const
{
	return pActorSprite_->getPositionY();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 Actor::GetPositionRealCenter() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Vec2 thisPos = getPosition();
	thisPos.x += pThicknessBox_->getPositionX();
	thisPos.y += pThicknessBox_->getPositionY();
	return thisPos;
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::GetPositionRealCenterX() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionX() + pThicknessBox_->getPositionX();
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::GetPositionRealCenterY() const
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionY() + pThicknessBox_->getPositionY();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 Actor::GetCanvasPositionReal() const
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	SGSize contentSize = pActorSprite_->GetBodyCanvas()->getContentSize();
	SGVec2 canvasRealPos = this->getPosition() - (contentSize / 2) + pActorSprite_->getPosition();
	return canvasRealPos;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGSize Actor::GetCanvasSize() const
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	return pActorSprite_->GetBodyCanvasSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGRect Actor::GetHitBox() const
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	// 위치: 캔버스 좌하단 절대 좌표 + 캔버스 좌하단 기준 스킨 파츠 좌표
	//      캔버스 좌하단 절대 좌표 = 플레이어 Cocos 위치  + 캐릭터 위치 - (캔버스 사이즈 / 2)
	Vec2 canvasPosition = GetCanvasPositionReal();
	Vec2 skinPartPos = pActorSprite_->GetBodyPartPosition(); // 캔버스 좌하단 기준 스킨 파츠 좌표
	Rect hitBox{ GetCanvasPositionReal() + skinPartPos, pActorSprite_->GetBodyPartSize() };
	return hitBox;
}

ActorSprite* Actor::GetActorSprite() const
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 초기화되지 않았습니다.");
	return pActorSprite_;
}

SpriteDirection_t Actor::GetSpriteDirection() const
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 초기화되지 않았습니다.");
	return pActorSprite_->GetSpriteDirection();
}

int Actor::GetRunningAnimationCode()
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 초기화되지 않았습니다.");
	return pActorSprite_->GetRunningAnimationCode();
}

ActorPartAnimation* Actor::GetRunningAnimation()
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 초기화되지 않았습니다.");
	return pActorSprite_->GetRunningAnimation();
}

void Actor::SetPositionReal(float _x, float _y)
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = GetThicknessBox();

	_x -= thicknessBox.relativeX_;
	_y -= thicknessBox.relativeY_;

	_x += thicknessBox.width_ / 2.0f;
	_y += thicknessBox.height_ / 2.0f;

	setPosition(_x, _y);
}

void Actor::SetPositionReal(const SGVec2& _v)
{
	SetPositionReal(_v.x, _v.y);
}

void Actor::SetPositionRealX(float _x)
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = GetThicknessBox();
	_x -= thicknessBox.relativeX_;
	_x += thicknessBox.width_ / 2.0f;
	setPositionX(_x);
}

void Actor::SetPositionRealY(float _y)
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = GetThicknessBox();
	_y -= thicknessBox.relativeY_;
	_y += thicknessBox.height_ / 2.0f;
	setPositionY(_y);
}

void Actor::SetPositionRealCenter(float _x, float _y)
{
	DebugAssertMsg(pThicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	setPosition(
		_x -= pThicknessBox_->getPositionX(),
		_y -= pThicknessBox_->getPositionY()
	);
}

void Actor::SetPositionRealCenter(const SGVec2& _v)
{
	SetPositionRealCenter(_v.x, _v.y);
}

void Actor::SetMapLayer(MapLayer* _pMapLayer)
{
	pMapLayer_ = _pMapLayer;
}

void Actor::SetActorId(int _id)
{
	// 청소되지 않은 액터에 할당을 시도할려는 경우를 막아야한다.
	if (!isCleanUp_)
	{
		DebugAssertMsg(actorId_ == InvalidValue_v, "이미 ID값이 할당되어 있습니다.");
	}

	actorId_ = _id;
}

void Actor::RunAnimation(int _animationCode)
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	pActorSprite_->RunAnimation(_animationCode);
}

void Actor::RunAnimation(int _animationCode, int _startFrameIndexInAnimation)
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	pActorSprite_->RunAnimation(_animationCode, _startFrameIndexInAnimation);
}

void Actor::PauseAnimation(float _delay)
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	pActorSprite_->PauseAnimation(_delay);
}

void Actor::RunFrameEventSpawn(FrameEventSpawnType_t _spawnType, int _spawnCode)
{
	switch (_spawnType)
	{
	case FrameEventSpawnType::Projectile:
		Core::Contents.ActorManager->CreateProejctileOnMap(this, _spawnCode);
		break;
	case FrameEventSpawnType::AttackBox:
		DebugAssertMsg(false, "미구현");
		break;
	default: DebugAssert(false);
	}
}

void Actor::RunFrameEvent(int _frameEventCode)
{
	if (_frameEventCode == InvalidValue_v)
		return;

	FrameEvent* pFrameEvent = Core::DataManager->GetFrameEvent(GetType(), _frameEventCode);

	if (pFrameEvent == nullptr)
		return;

	RunFrameEvent(pFrameEvent);
}

void Actor::RunFrameEvent(FrameEvent* _pFrameEvent)
{
	switch (_pFrameEvent->type_)
	{
	case FrameEventType::Spawn:
		{
			const FrameEventSpawn* pSpawn = dynamic_cast<FrameEventSpawn*>(_pFrameEvent);

			if (pSpawn == nullptr)
			{
				DebugAssert(false);
				break;
			}
			RunFrameEventSpawn(pSpawn->spawnType_, pSpawn->spawnCode_);
			break;
		}
	case FrameEventType::AttackBoxInstant:
		{
			const FrameEventAttackBoxInstant* pAttackBoxInstant = dynamic_cast<FrameEventAttackBoxInstant*>(
				_pFrameEvent);

			if (pAttackBoxInstant == nullptr)
			{
				DebugAssert(false);
				break;
			}

			if (pHitRecorder_ == nullptr)
				break;

			pHitRecorder_->Record(pAttackBoxInstant);
			break;
		}
	default:
		DebugAssert(false);
	}
}

void Actor::OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	listeners_.OnFrameBegin(_pAnimation, _pTexture);
	RunFrameEvent(_pAnimation->GetRunningFrameEventCode());

	AIActivity* pRunningActivity = GetRunningAiActivity();

	if (pRunningActivity)
	{
		pRunningActivity->OnFrameBegin(_pAnimation, _pTexture);
	}
}

void Actor::OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	listeners_.OnFrameEnd(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = GetRunningAiActivity();

	if (pRunningActivity)
	{
		pRunningActivity->OnFrameEnd(_pAnimation, _pTexture);
	}
}

void Actor::OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	listeners_.OnAnimationBegin(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = GetRunningAiActivity();

	if (pRunningActivity)
	{
		pRunningActivity->OnAnimationBegin(_pAnimation, _pTexture);
	}
}

void Actor::OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	listeners_.OnAnimationEnd(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = GetRunningAiActivity();

	if (pRunningActivity)
	{
		pRunningActivity->OnAnimationEnd(_pAnimation, _pTexture);
	}
}

void Actor::SetSpriteDirection(SpriteDirection_t _direction)
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	pActorSprite_->SetSpriteDirection(_direction);
}

void Actor::SetForwardDirection()
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	pActorSprite_->SetForwardDirection();
}

void Actor::SetBackwardDirection()
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	pActorSprite_->SetBackwardDirection();
}

bool Actor::IsCollide(Actor* _pOther, SpriteDirection_t& _otherHitDirection, SGRect& _hitRect)
{
	SGRect myThick = GetThicknessBoxRect();
	SGRect otherBox = _pOther->GetThicknessBoxRect();

	if (!RectEx::IntersectY(myThick, otherBox))
	{
		return false;
	}

	SGRect myHit = GetHitBox();
	SGRect targetHit = _pOther->GetHitBox();

	if (RectEx::Intersect(myHit, targetHit, _hitRect))
	{
		_otherHitDirection = targetHit.getMidX() > myHit.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		return true;
	}

	return false;
}

bool Actor::IsCollide(const ActorRect& _otherRect, SpriteDirection_t& _otherHitDirection, SGRect& _hitRect)
{
	SGRect myThick = GetThicknessBoxRect();

	if (!RectEx::IntersectY(myThick, _otherRect.thicknessRect_))
	{
		return false;
	}

	SGRect myHit = GetHitBox();

	if (RectEx::Intersect(myHit, _otherRect.bodyRect_, _hitRect))
	{
		_otherHitDirection = _otherRect.bodyRect_.getMidX() > myHit.getMidX()
			                     ? SpriteDirection::Left
			                     : SpriteDirection::Right;
		return true;
	}

	return false;
}

bool Actor::IsCollide(const ActorRect& _otherRect)
{
	SGRect myThick = GetThicknessBoxRect();

	if (!RectEx::IntersectY(myThick, _otherRect.thicknessRect_))
	{
		return false;
	}

	SGRect myHit = GetHitBox();
	return myHit.intersectsRect(_otherRect.bodyRect_);
}

bool Actor::IsOnTheGround()
{
	DebugAssertMsg(pActorSprite_, "액터 스프라이트가 없습니다.");
	return pActorSprite_->getPositionY() <= 0;
}

void Actor::ReleaseActorSprite()
{
	if (!pActorSprite_)
		return;

	removeChild(pActorSprite_);
	pActorSprite_ = nullptr;
}

void Actor::CleanUpAtNextFrame()
{
	DebugAssertMsg(pMapLayer_, "소속된 맵이 존재하지 않습니다.");
	Core::Contents.ActorManager->CleanUpAtNextFrame(this);
	isCleanUp_ = true;
}

void Actor::CleanUp()
{
	isCleanUp_ = true;
	pMapLayer_ = nullptr;
	Core::Contents.ActorManager->CleanUp(this);
}

void Actor::Attach(Actor* _pActor)
{
	DebugAssertMsg(_pActor->HasAttacher() == false, "어태치 할려는 액터가 이미 다른 누군가에게 어태치 되어 있습니다.");
	DebugAssertMsg(attches_.Exist(_pActor) == false, "동일한 액터에 대해서 연속 어태치 할 수 없습니다.");

	attches_.PushBack(_pActor);

	_pActor->pAttacher_ = this;
	_pActor->retain();
}

void Actor::Detach(Actor* _pActor)
{
	DebugAssertMsg(attches_.Exist(_pActor), "디태치 할려는 액터가 어태치되어 있지 않습니다.");
	if (attches_.Remove(_pActor) == false)
	{
		DebugAssertMsg(false, "디태치 실패");
	}
}

Actor* Actor::GetAttacher()
{
	return pAttacher_;
}

bool Actor::HasAttacher()
{
	return pAttacher_ != nullptr;
}

bool Actor::HasCleanUpFlag(int _cleanUpFlag)
{
	return (cleanUpFlag_ & _cleanUpFlag) == _cleanUpFlag;
}

void Actor::AddCleanUpFlag(int _cleanUpFlag)
{
	cleanUpFlag_ |= _cleanUpFlag;
}

void Actor::ClearCleanUpFlag()
{
	cleanUpFlag_ = cfNone;
}

ActorRect Actor::ConvertAbsoluteActorRect(Actor* _pStdActor, const ActorRect& _relativeRect)
{
	ActorRect absoluteActorRect;
	const SGSize spawnerCanvasSize = _pStdActor->GetCanvasSize();
	const SGVec2 spawnerCanvasPos = _pStdActor->GetCanvasPositionReal();

	if (_pStdActor->GetSpriteDirection() == SpriteDirection::Right)
	{
		absoluteActorRect.thicknessRect_.origin.x = spawnerCanvasPos.x + _relativeRect.thicknessRect_.origin.x;
		absoluteActorRect.thicknessRect_.origin.y = spawnerCanvasPos.y + _relativeRect.thicknessRect_.origin.y;

		absoluteActorRect.bodyRect_.origin.x = spawnerCanvasPos.x + _relativeRect.bodyRect_.origin.x;
		absoluteActorRect.bodyRect_.origin.y = spawnerCanvasPos.y + _relativeRect.bodyRect_.origin.y;
	}
	else
	{
		absoluteActorRect.thicknessRect_.origin.x = spawnerCanvasPos.x + (spawnerCanvasSize.width - _relativeRect.
			thicknessRect_.origin.x - _relativeRect.thicknessRect_.size.width);
		absoluteActorRect.thicknessRect_.origin.y = spawnerCanvasPos.y + _relativeRect.thicknessRect_.origin.y;

		absoluteActorRect.bodyRect_.origin.x = spawnerCanvasPos.x + (spawnerCanvasSize.width - _relativeRect.bodyRect_.
			origin.x - _relativeRect.bodyRect_.size.width);
		absoluteActorRect.bodyRect_.origin.y = spawnerCanvasPos.y + _relativeRect.bodyRect_.origin.y;
	}

	absoluteActorRect.thicknessRect_.size = _relativeRect.thicknessRect_.size;
	absoluteActorRect.bodyRect_.size = _relativeRect.bodyRect_.size;

	return absoluteActorRect;
}

AIActivity* Actor::GetRunningAiActivity()
{
	const AIComponent* pAIComponent = components_.Get<AIComponent>();

	if (pAIComponent == nullptr)
	{
		return nullptr;
	}

	return pAIComponent->GetRunningActivity();
}
