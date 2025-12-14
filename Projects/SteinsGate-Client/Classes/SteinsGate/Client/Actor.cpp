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
	: mapLayer_(nullptr)
	, actorSprite_(nullptr)
	, hitRecorder_(nullptr)
	, actorId_(InvalidValue_v)
	, allyFlag_(0)
	, cleanUpFlag_(CF_None)
	, cleanUp_(false)
	, thicknessBox_(nullptr)
	, attaches_(4)
	, attacher_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Actor::~Actor()
{
	JCORE_DELETE_SAFE(hitRecorder_);
	listeners_.deleteAll();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::initVariables()
{
	// 이거 2개만 초기화 시키면 댈듯?
	actorId_ = InvalidValue_v;
	mapLayer_ = nullptr;
	attaches_.Clear();
	attacher_ = nullptr;
	cleanUp_ = false;
	cleanUpFlag_ = CF_None;

	// m_eActorType = ;
	// m_iCode = ;
	// thicknessBox_ = nullptr;
	// allyFlag_ = 0;
	// hitRecorder_ = nullptr;
	// actorSprite_ = nullptr;
	// m_pListener = nullptr;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::addListener(IActorListener* _pListener)
{
	return listeners_.add(_pListener);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::hasListener(IActorListener::Type _type)
{
	return listeners_.has(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
IActorListener* Actor::getListener(IActorListener::Type _type)
{
	return listeners_.get(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::initThicknessBox(const ThicknessBox& _thicknessBox)
{
	// DrawNode는 앵커포인트 신경안쓰고 컨텐츠박스 기준 좌하단부터 그림
	RectPoly poly = RectPoly::createFromLeftBottom(0, 0, _thicknessBox.Width, _thicknessBox.Height);

	if (thicknessBox_ == nullptr)
	{
		thicknessBox_ = SGDrawNode::create();
		thicknessBox_->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		thicknessBox_->setOpacity(125);
		thicknessBox_->drawPolygon(poly.source(), 4, {}, 1, SGColor4F{ ColorList::Brightgreen_v });
		this->addChild(thicknessBox_);
	}
	thicknessBox_->setPositionX(_thicknessBox.RelativeX);
	thicknessBox_->setPositionY(_thicknessBox.RelativeY);
	thicknessBox_->setContentSize(_thicknessBox.Width, _thicknessBox.Height);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::initHitRecorder(int _hitPossibleListSize, int _alreadyHitMapSize, Actor* _pOwner)
{
	Actor* pOwner = _pOwner;

	if (pOwner == nullptr)
		pOwner = this;

	if (hitRecorder_ == nullptr)
		hitRecorder_ = dbg_new HitRecorder(pOwner, this, _hitPossibleListSize, _alreadyHitMapSize);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::hit(const HitInfo& _hitInfo)
{
	PhysicsComponent* pPhysicsComponent = getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
	{
		pPhysicsComponent->hit(_hitInfo);
	}

	getListenerCollection().onHit(_hitInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::update(float _dt)
{
	DebugAssertMsg(cleanUp_ == false, "다음 프레임에 풀로 복귀 예정중인 객체입니다.");
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	DebugAssertMsg(mapLayer_, "맵 레이어가 세팅되지 않았습니다.");

	actorSprite_->update(_dt);
	listeners_.onUpdate(_dt);
	components_.onUpdate(_dt);

	if (Core::Contents.Global->DrawThicknessBox)
		thicknessBox_->setOpacity(0);
	else
		thicknessBox_->setOpacity(125);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::addComponent(IComponent* _pComponent)
{
	components_.add(_pComponent);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::hasComponent(IComponent::Type _type) const
{
	return components_.has(_type);
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* Actor::getTypeName() const
{
	return ActorType::Name[getType()];
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorRect Actor::getActorRect() const
{
	return { getThicknessBoxRect(), getHitBox() };
}

//////////////////////////////////////////////////////////////////////////////////////////
ThicknessBox Actor::getThicknessBox() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");

	SGVec2 pos = thicknessBox_->getPosition();
	SGSize size = thicknessBox_->getContentSize();

	return { pos.x, pos.y, size.width, size.height };
}

//////////////////////////////////////////////////////////////////////////////////////////
SGDrawNode* Actor::getThicknessBoxNode() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return thicknessBox_;
}

//////////////////////////////////////////////////////////////////////////////////////////
Rect Actor::getThicknessBoxRect() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { getPositionReal(), thicknessBox_->getContentSize() };
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 Actor::getPositionReal() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	thicknessOrigin.y += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOrigin;
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::getPositionRealX() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginX = getPositionX();
	thicknessOriginX += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	return thicknessOriginX;
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::getPositionRealY() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginY = getPositionY();
	thicknessOriginY += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOriginY;
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::getPositionActorY() const
{
	return actorSprite_->getPositionY();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 Actor::getPositionRealCenter() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Vec2 thisPos = getPosition();
	thisPos.x += thicknessBox_->getPositionX();
	thisPos.y += thicknessBox_->getPositionY();
	return thisPos;
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::getPositionRealCenterX() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionX() + thicknessBox_->getPositionX();
}

//////////////////////////////////////////////////////////////////////////////////////////
float Actor::getPositionRealCenterY() const
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionY() + thicknessBox_->getPositionY();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGVec2 Actor::getCanvasPositionReal() const
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	SGSize contentSize = actorSprite_->getBodyCanvas()->getContentSize();
	SGVec2 canvasRealPos = this->getPosition() - (contentSize / 2) + actorSprite_->getPosition();
	return canvasRealPos;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGSize Actor::getCanvasSize() const
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	return actorSprite_->getBodyCanvasSize();
}

//////////////////////////////////////////////////////////////////////////////////////////
SGRect Actor::getHitBox() const
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	// 위치: 캔버스 좌하단 절대 좌표 + 캔버스 좌하단 기준 스킨 파츠 좌표
	//      캔버스 좌하단 절대 좌표 = 플레이어 Cocos 위치  + 캐릭터 위치 - (캔버스 사이즈 / 2)
	Vec2 canvasPosition = getCanvasPositionReal();
	Vec2 skinPartPos = actorSprite_->getBodyPartPosition(); // 캔버스 좌하단 기준 스킨 파츠 좌표
	Rect hitBox{ getCanvasPositionReal() + skinPartPos, actorSprite_->getBodyPartSize() };
	return hitBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorSprite* Actor::getActorSprite() const
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 초기화되지 않았습니다.");
	return actorSprite_;
}

//////////////////////////////////////////////////////////////////////////////////////////
SpriteDirection_t Actor::getSpriteDirection() const
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 초기화되지 않았습니다.");
	return actorSprite_->getSpriteDirection();
}

//////////////////////////////////////////////////////////////////////////////////////////
int Actor::getRunningAnimationCode()
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 초기화되지 않았습니다.");
	return actorSprite_->getRunningAnimationCode();
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartAnimation* Actor::getRunningAnimation()
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 초기화되지 않았습니다.");
	return actorSprite_->getRunningAnimation();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setPositionReal(float _x, float _y)
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();

	_x -= thicknessBox.RelativeX;
	_y -= thicknessBox.RelativeY;

	_x += thicknessBox.Width / 2.0f;
	_y += thicknessBox.Height / 2.0f;

	setPosition(_x, _y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setPositionReal(const SGVec2& _position)
{
	setPositionReal(_position.x, _position.y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setPositionRealX(float _x)
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	_x -= thicknessBox.RelativeX;
	_x += thicknessBox.Width / 2.0f;
	setPositionX(_x);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setPositionRealY(float _y)
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	_y -= thicknessBox.RelativeY;
	_y += thicknessBox.Height / 2.0f;
	setPositionY(_y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setPositionRealCenter(float _x, float _y)
{
	DebugAssertMsg(thicknessBox_, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	setPosition(
		_x -= thicknessBox_->getPositionX(),
		_y -= thicknessBox_->getPositionY());
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setPositionRealCenter(const SGVec2& _position)
{
	setPositionRealCenter(_position.x, _position.y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setMapLayer(MapLayer* _pMapLayer)
{
	mapLayer_ = _pMapLayer;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setActorId(int _id)
{
	// 청소되지 않은 액터에 할당을 시도할려는 경우를 막아야한다.
	if (!cleanUp_)
	{
		DebugAssertMsg(actorId_ == InvalidValue_v, "이미 ID값이 할당되어 있습니다.");
	}

	actorId_ = _id;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::runAnimation(int _animationCode)
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	actorSprite_->runAnimation(_animationCode);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::runAnimation(int _animationCode, int _startFrameIndexInAnimation)
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	actorSprite_->runAnimation(_animationCode, _startFrameIndexInAnimation);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::pauseAnimation(float _delay)
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	actorSprite_->pauseAnimation(_delay);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::runFrameEventSpawn(FrameEventSpawnType_t _frameEventSpawnType, int _code)
{
	switch (_frameEventSpawnType)
	{
	case FrameEventSpawnType::Projectile:
		Core::Contents.ActorManager->createProejctileOnMap(this, _code);
		break;
	case FrameEventSpawnType::AttackBox:
		DebugAssertMsg(false, "미구현");
		break;
	default:
		DebugAssert(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::runFrameEvent(int _frameEventCode)
{
	if (_frameEventCode == InvalidValue_v)
		return;

	FrameEvent* pFrameEvent = Core::DataManager->getFrameEvent(getType(), _frameEventCode);

	if (pFrameEvent == nullptr)
		return;

	runFrameEvent(pFrameEvent);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::runFrameEvent(FrameEvent* _pFrameEvent)
{
	switch (_pFrameEvent->Type)
	{
	case FrameEventType::Spawn:
	{
		const FrameEventSpawn* pSpawn = dynamic_cast<FrameEventSpawn*>(_pFrameEvent);

		if (pSpawn == nullptr)
		{
			DebugAssert(false);
			break;
		}
		runFrameEventSpawn(pSpawn->SpawnType, pSpawn->SpawnCode);
		break;
	}
	case FrameEventType::AttackBoxInstant:
	{
		const FrameEventAttackBoxInstant* pAttackBoxInstant = dynamic_cast<FrameEventAttackBoxInstant*>(_pFrameEvent);

		if (pAttackBoxInstant == nullptr)
		{
			DebugAssert(false);
			break;
		}

		if (hitRecorder_ == nullptr)
			break;

		hitRecorder_->record(pAttackBoxInstant);
		break;
	}
	default:
		DebugAssert(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	listeners_.onFrameBegin(_pAnimation, _pTexture);
	runFrameEvent(_pAnimation->getRunningFrameEventCode());

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity)
	{
		pRunningActivity->onFrameBegin(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	listeners_.onFrameEnd(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity)
	{
		pRunningActivity->onFrameEnd(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	listeners_.onAnimationBegin(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity)
	{
		pRunningActivity->onAnimationBegin(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	listeners_.onAnimationEnd(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity)
	{
		pRunningActivity->onAnimationEnd(_pAnimation, _pTexture);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setSpriteDirection(SpriteDirection_t _direction)
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	actorSprite_->setSpriteDirection(_direction);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setForwardDirection()
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	actorSprite_->setForwardDirection();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::setBackwardDirection()
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	actorSprite_->setBackwardDirection();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::isCollide(Actor* _pOther, JCORE_OUT SpriteDirection_t& _otherHitDirection, JCORE_OUT SGRect& _hitRect)
{
	SGRect myThick = getThicknessBoxRect();
	SGRect otherBox = _pOther->getThicknessBoxRect();

	if (!RectEx::intersectY(myThick, otherBox))
	{
		return false;
	}

	SGRect myHit = getHitBox();
	SGRect targetHit = _pOther->getHitBox();

	if (RectEx::intersect(myHit, targetHit, _hitRect))
	{
		_otherHitDirection = targetHit.getMidX() > myHit.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::isCollide(const ActorRect& _otherRect, SpriteDirection_t& _otherHitDirection, SGRect& _hitRect)
{
	SGRect myThick = getThicknessBoxRect();

	if (!RectEx::intersectY(myThick, _otherRect.ThicknessRect))
	{
		return false;
	}

	SGRect myHit = getHitBox();

	if (RectEx::intersect(myHit, _otherRect.BodyRect, _hitRect))
	{
		_otherHitDirection = _otherRect.BodyRect.getMidX() > myHit.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::isCollide(const ActorRect& _otherRect)
{
	SGRect myThick = getThicknessBoxRect();

	if (!RectEx::intersectY(myThick, _otherRect.ThicknessRect))
	{
		return false;
	}

	SGRect myHit = getHitBox();
	return myHit.intersectsRect(_otherRect.BodyRect);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::isOnTheGround()
{
	DebugAssertMsg(actorSprite_, "액터 스프라이트가 없습니다.");
	return actorSprite_->getPositionY() <= 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::releaseActorSprite()
{
	if (!actorSprite_)
	{
		return;
	}

	removeChild(actorSprite_);
	actorSprite_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::cleanUpAtNextFrame()
{
	DebugAssertMsg(mapLayer_, "소속된 맵이 존재하지 않습니다.");
	Core::Contents.ActorManager->cleanUpAtNextFrame(this);
	cleanUp_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::cleanUp()
{
	cleanUp_ = true;
	mapLayer_ = nullptr;
	Core::Contents.ActorManager->cleanUp(this);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::attach(Actor* _pActor)
{
	DebugAssertMsg(_pActor->hasAttacher() == false, "어태치 할려는 액터가 이미 다른 누군가에게 어태치 되어 있습니다.");
	DebugAssertMsg(attaches_.Exist(_pActor) == false, "동일한 액터에 대해서 연속 어태치 할 수 없습니다.");

	attaches_.PushBack(_pActor);

	_pActor->attacher_ = this;
	_pActor->retain();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::detach(Actor* _pActor)
{
	DebugAssertMsg(attaches_.Exist(_pActor), "디태치 할려는 액터가 어태치되어 있지 않습니다.");
	if (attaches_.Remove(_pActor) == false)
	{
		DebugAssertMsg(false, "디태치 실패");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
Actor* Actor::getAttacher()
{
	return attacher_;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::hasAttacher()
{
	return attacher_ != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Actor::hasCleanUpFlag(int _cleanUpFlag)
{
	return (cleanUpFlag_ & _cleanUpFlag) == _cleanUpFlag;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::addCleanUpFlag(int _cleanUpFlag)
{
	cleanUpFlag_ |= _cleanUpFlag;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Actor::clearCleanUpFlag()
{
	cleanUpFlag_ = CF_None;
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorRect Actor::convertAbsoluteActorRect(Actor* _pStdActor, const ActorRect& _relativeRect)
{
	ActorRect absoluteActorRect;
	const SGSize spawnerCanvasSize = _pStdActor->getCanvasSize();
	const SGVec2 spawnerCanvasPos = _pStdActor->getCanvasPositionReal();

	if (_pStdActor->getSpriteDirection() == SpriteDirection::Right)
	{
		absoluteActorRect.ThicknessRect.origin.x = spawnerCanvasPos.x + _relativeRect.ThicknessRect.origin.x;
		absoluteActorRect.ThicknessRect.origin.y = spawnerCanvasPos.y + _relativeRect.ThicknessRect.origin.y;

		absoluteActorRect.BodyRect.origin.x = spawnerCanvasPos.x + _relativeRect.BodyRect.origin.x;
		absoluteActorRect.BodyRect.origin.y = spawnerCanvasPos.y + _relativeRect.BodyRect.origin.y;
	}
	else
	{
		absoluteActorRect.ThicknessRect.origin.x = spawnerCanvasPos.x + (spawnerCanvasSize.width - _relativeRect.ThicknessRect.origin.x - _relativeRect.ThicknessRect.size.width);
		absoluteActorRect.ThicknessRect.origin.y = spawnerCanvasPos.y + _relativeRect.ThicknessRect.origin.y;

		absoluteActorRect.BodyRect.origin.x = spawnerCanvasPos.x + (spawnerCanvasSize.width - _relativeRect.BodyRect.origin.x - _relativeRect.BodyRect.size.width);
		absoluteActorRect.BodyRect.origin.y = spawnerCanvasPos.y + _relativeRect.BodyRect.origin.y;
	}

	absoluteActorRect.ThicknessRect.size = _relativeRect.ThicknessRect.size;
	absoluteActorRect.BodyRect.size = _relativeRect.BodyRect.size;

	return absoluteActorRect;
}

//////////////////////////////////////////////////////////////////////////////////////////
AIActivity* Actor::getRunningAIActivity()
{
	const AIComponent* pAIComponent = components_.get<AIComponent>();

	if (pAIComponent == nullptr)
	{
		return nullptr;
	}

	return pAIComponent->getRunningActivity();
}
