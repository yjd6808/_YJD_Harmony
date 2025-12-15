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

Actor::Actor()
: m_pMapLayer(nullptr)
, m_pActorSprite(nullptr)
, m_pHitRecorder(nullptr)
, m_iActorId(InvalidValue_v)
, m_iAllyFlag(0)
, m_iCleanUpFlag(CF_None)
, m_bCleanUp(false)
, m_pThicknessBox(nullptr)
, m_pAttacher(nullptr)
, m_vAttches(4)
{
}

Actor::~Actor()
{
	JCORE_DELETE_SAFE(m_pHitRecorder);
	m_Listeners.deleteAll();
}


bool Actor::initVariables()
{
	// 이거 2개만 초기화 시키면 댈듯?
	m_iActorId = InvalidValue_v;
	m_pMapLayer = nullptr;
	m_vAttches.Clear();
	m_pAttacher = nullptr;
	m_bCleanUp = false;
	m_iCleanUpFlag = CF_None;

	// m_eActorType = ;
	// m_iCode = ;
	// m_pThicknessBox = nullptr;
	// m_iAllyFlag = 0;
	// m_pHitRecorder = nullptr;
	// m_pActorSprite = nullptr;
	// m_pListener = nullptr;

	return true;
}

bool Actor::addListener(IActorListener* _pListener)
{
	return m_Listeners.add(_pListener);
}

bool Actor::hasListener(IActorListener::Type _type)
{
	return m_Listeners.has(_type);
}

IActorListener* Actor::getListener(IActorListener::Type _type)
{
	return m_Listeners.get(_type);
}

void Actor::initThicknessBox(const ThicknessBox& _thicknessBox)
{
	// DrawNode는 앵커포인트 신경안쓰고 컨텐츠박스 기준 좌하단부터 그림
	RectPoly poly = RectPoly::createFromLeftBottom(0, 0, _thicknessBox.Width, _thicknessBox.Height);

	if (m_pThicknessBox == nullptr)
	{
		m_pThicknessBox = SGDrawNode::create();
		m_pThicknessBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pThicknessBox->setOpacity(125);
		m_pThicknessBox->drawPolygon(poly.source(), 4, {}, 1, SGColor4F{ ColorList::Brightgreen_v });
		this->addChild(m_pThicknessBox);
	}
	m_pThicknessBox->setPositionX(_thicknessBox.RelativeX);
	m_pThicknessBox->setPositionY(_thicknessBox.RelativeY);
	m_pThicknessBox->setContentSize(_thicknessBox.Width, _thicknessBox.Height);
}

void Actor::initHitRecorder(int _hitPossibleListSize /* = 16 */, int _alreadyHitMapSize /* = 32 */,
                            Actor* _pOwner /*= nullptr */)
{
	Actor* pOwner = _pOwner;

	if (pOwner == nullptr)
		pOwner = this;

	if (m_pHitRecorder == nullptr)
		m_pHitRecorder = dbg_new HitRecorder(pOwner, this, _hitPossibleListSize, _alreadyHitMapSize);
}

void Actor::hit(const HitInfo& _hitInfo)
{
	PhysicsComponent* pPhysicsComponent = getComponent<PhysicsComponent>();

	if (pPhysicsComponent)
	{
		pPhysicsComponent->hit(_hitInfo);
	}

	getListenerCollection().onHit(_hitInfo);
}

void Actor::update(float _dt)
{
	DebugAssertMsg(m_bCleanUp == false, "다음 프레임에 풀로 복귀 예정중인 객체입니다.");
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	DebugAssertMsg(m_pMapLayer, "맵 레이어가 세팅되지 않았습니다.");

	m_pActorSprite->update(_dt);
	m_Listeners.onUpdate(_dt);
	m_Components.onUpdate(_dt);

	if (Core::Contents.Global->DrawThicknessBox)
		m_pThicknessBox->setOpacity(0);
	else
		m_pThicknessBox->setOpacity(125);
}

void Actor::addComponent(IComponent* _pComponenet)
{
	m_Components.add(_pComponenet);
}

bool Actor::hasComponent(IComponent::Type _type) const
{
	return m_Components.has(_type);
}

const char* Actor::getTypeName() const
{
	return ActorType::Name[getType()];
}

ActorRect Actor::getActorRect() const
{
	return { getThicknessBoxRect(), getHitBox() };
}

ThicknessBox Actor::getThicknessBox() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");

	SGVec2 pos = m_pThicknessBox->getPosition();
	SGSize size = m_pThicknessBox->getContentSize();

	return { pos.x, pos.y, size.width, size.height };
}

SGDrawNode* Actor::getThicknessBoxNode() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return m_pThicknessBox;
}

Rect Actor::getThicknessBoxRect() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { getPositionReal(), m_pThicknessBox->getContentSize() };
}

SGVec2 Actor::getPositionReal() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	thicknessOrigin.y += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOrigin;
}

float Actor::getPositionRealX() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginX = getPositionX();
	thicknessOriginX += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	return thicknessOriginX;
}

float Actor::getPositionRealY() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginY = getPositionY();
	thicknessOriginY += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOriginY;
}

float Actor::getPositionActorY() const
{
	return m_pActorSprite->getPositionY();
}

SGVec2 Actor::getPositionRealCenter() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Vec2 thisPos = getPosition();
	thisPos.x += m_pThicknessBox->getPositionX();
	thisPos.y += m_pThicknessBox->getPositionY();
	return thisPos;
}

float Actor::getPositionRealCenterX() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionX() + m_pThicknessBox->getPositionX();
}

float Actor::getPositionRealCenterY() const
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionY() + m_pThicknessBox->getPositionY();
}

SGVec2 Actor::getCanvasPositionReal() const
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	SGSize contentSize = m_pActorSprite->getBodyCanvas()->getContentSize();
	SGVec2 canvasRealPos = this->getPosition() - (contentSize / 2) + m_pActorSprite->getPosition();
	return canvasRealPos;
}

SGSize Actor::getCanvasSize() const
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getBodyCanvasSize();
}

SGRect Actor::getHitBox() const
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	// 위치: 캔버스 좌하단 절대 좌표 + 캔버스 좌하단 기준 스킨 파츠 좌표
	//      캔버스 좌하단 절대 좌표 = 플레이어 Cocos 위치  + 캐릭터 위치 - (캔버스 사이즈 / 2)
	Vec2 canvasPosition = getCanvasPositionReal();
	Vec2 skinPartPos = m_pActorSprite->getBodyPartPosition(); // 캔버스 좌하단 기준 스킨 파츠 좌표
	Rect hitBox{ getCanvasPositionReal() + skinPartPos, m_pActorSprite->getBodyPartSize() };
	return hitBox;
}

ActorSprite* Actor::getActorSprite() const
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite;
}

SpriteDirection_t Actor::getSpriteDirection() const
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getSpriteDirection();
}

int Actor::getRunningAnimationCode()
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getRunningAnimationCode();
}

ActorPartAnimation* Actor::getRunningAnimation()
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getRunningAnimation();
}

void Actor::setPositionReal(float _x, float _y)
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();

	_x -= thicknessBox.RelativeX;
	_y -= thicknessBox.RelativeY;

	_x += thicknessBox.Width / 2.0f;
	_y += thicknessBox.Height / 2.0f;

	setPosition(_x, _y);
}

void Actor::setPositionReal(const SGVec2& _v)
{
	setPositionReal(_v.x, _v.y);
}

void Actor::setPositionRealX(float _x)
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	_x -= thicknessBox.RelativeX;
	_x += thicknessBox.Width / 2.0f;
	setPositionX(_x);
}

void Actor::setPositionRealY(float _y)
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	_y -= thicknessBox.RelativeY;
	_y += thicknessBox.Height / 2.0f;
	setPositionY(_y);
}

void Actor::setPositionRealCenter(float _x, float _y)
{
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	setPosition(
		_x -= m_pThicknessBox->getPositionX(),
		_y -= m_pThicknessBox->getPositionY()
	);
}

void Actor::setPositionRealCenter(const SGVec2& _v)
{
	setPositionRealCenter(_v.x, _v.y);
}

void Actor::setMapLayer(MapLayer* _pMapLayer)
{
	m_pMapLayer = _pMapLayer;
}

void Actor::setActorId(int _id)
{
	// 청소되지 않은 액터에 할당을 시도할려는 경우를 막아야한다.
	if (!m_bCleanUp)
	{
		DebugAssertMsg(m_iActorId == InvalidValue_v, "이미 ID값이 할당되어 있습니다.");
	}

	m_iActorId = _id;
}

void Actor::runAnimation(int _animationCode)
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(_animationCode);
}

void Actor::runAnimation(int _animationCode, int _startFrameIndexInAnimation)
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(_animationCode, _startFrameIndexInAnimation);
}

void Actor::pauseAnimation(float _delay)
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->pauseAnimation(_delay);
}

void Actor::runFrameEventSpawn(FrameEventSpawnType_t _spawnType, int _spawnCode)
{
	switch (_spawnType)
	{
	case FrameEventSpawnType::Projectile:
		Core::Contents.ActorManager->createProejctileOnMap(this, _spawnCode);
		break;
	case FrameEventSpawnType::AttackBox:
		DebugAssertMsg(false, "미구현");
		break;
	default: DebugAssert(false);
	}
}

void Actor::runFrameEvent(int _frameEventCode)
{
	if (_frameEventCode == InvalidValue_v)
		return;

	FrameEvent* pFrameEvent = Core::DataManager->getFrameEvent(getType(), _frameEventCode);

	if (pFrameEvent == nullptr)
		return;

	runFrameEvent(pFrameEvent);
}

void Actor::runFrameEvent(FrameEvent* _pFrameEvent)
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
			runFrameEventSpawn(pSpawn->spawnType_, pSpawn->spawnCode_);
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

			if (m_pHitRecorder == nullptr)
				break;

			m_pHitRecorder->record(pAttackBoxInstant);
			break;
		}
	default:
		DebugAssert(false);
	}
}

void Actor::onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	m_Listeners.onFrameBegin(_pAnimation, _pTexture);
	runFrameEvent(_pAnimation->getRunningFrameEventCode());

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity)
	{
		pRunningActivity->OnFrameBegin(_pAnimation, _pTexture);
	}
}

void Actor::onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	m_Listeners.onFrameEnd(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity)
	{
		pRunningActivity->OnFrameEnd(_pAnimation, _pTexture);
	}
}

void Actor::onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	m_Listeners.onAnimationBegin(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity)
	{
		pRunningActivity->OnAnimationBegin(_pAnimation, _pTexture);
	}
}

void Actor::onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture)
{
	m_Listeners.onAnimationEnd(_pAnimation, _pTexture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity)
	{
		pRunningActivity->OnAnimationEnd(_pAnimation, _pTexture);
	}
}

void Actor::setSpriteDirection(SpriteDirection_t _direction)
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setSpriteDirection(_direction);
}

void Actor::setForwardDirection()
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setForwardDirection();
}

void Actor::setBackwardDirection()
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setBackwardDirection();
}

bool Actor::isCollide(Actor* _pOther, SpriteDirection_t& _otherHitDirection, SGRect& _hitRect)
{
	SGRect myThick = getThicknessBoxRect();
	SGRect otherBox = _pOther->getThicknessBoxRect();

	if (!RectEx::IntersectY(myThick, otherBox))
	{
		return false;
	}

	SGRect myHit = getHitBox();
	SGRect targetHit = _pOther->getHitBox();

	if (RectEx::Intersect(myHit, targetHit, _hitRect))
	{
		_otherHitDirection = targetHit.getMidX() > myHit.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		return true;
	}

	return false;
}

bool Actor::isCollide(const ActorRect& _otherRect, SpriteDirection_t& _otherHitDirection, SGRect& _hitRect)
{
	SGRect myThick = getThicknessBoxRect();

	if (!RectEx::IntersectY(myThick, _otherRect.ThicknessRect))
	{
		return false;
	}

	SGRect myHit = getHitBox();

	if (RectEx::Intersect(myHit, _otherRect.BodyRect, _hitRect))
	{
		_otherHitDirection = _otherRect.BodyRect.getMidX() > myHit.getMidX()
			                     ? SpriteDirection::Left
			                     : SpriteDirection::Right;
		return true;
	}

	return false;
}

bool Actor::isCollide(const ActorRect& _otherRect)
{
	SGRect myThick = getThicknessBoxRect();

	if (!RectEx::IntersectY(myThick, _otherRect.ThicknessRect))
	{
		return false;
	}

	SGRect myHit = getHitBox();
	return myHit.intersectsRect(_otherRect.BodyRect);
}

bool Actor::isOnTheGround()
{
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getPositionY() <= 0;
}

void Actor::releaseActorSprite()
{
	if (!m_pActorSprite)
		return;

	removeChild(m_pActorSprite);
	m_pActorSprite = nullptr;
}

void Actor::cleanUpAtNextFrame()
{
	DebugAssertMsg(m_pMapLayer, "소속된 맵이 존재하지 않습니다.");
	Core::Contents.ActorManager->cleanUpAtNextFrame(this);
	m_bCleanUp = true;
}

void Actor::cleanUp()
{
	m_bCleanUp = true;
	m_pMapLayer = nullptr;
	Core::Contents.ActorManager->cleanUp(this);
}

void Actor::attach(Actor* _pActor)
{
	DebugAssertMsg(_pActor->hasAttacher() == false, "어태치 할려는 액터가 이미 다른 누군가에게 어태치 되어 있습니다.");
	DebugAssertMsg(m_vAttches.Exist(_pActor) == false, "동일한 액터에 대해서 연속 어태치 할 수 없습니다.");

	m_vAttches.PushBack(_pActor);

	_pActor->m_pAttacher = this;
	_pActor->retain();
}

void Actor::detach(Actor* _pActor)
{
	DebugAssertMsg(m_vAttches.Exist(_pActor), "디태치 할려는 액터가 어태치되어 있지 않습니다.");
	if (m_vAttches.Remove(_pActor) == false)
	{
		DebugAssertMsg(false, "디태치 실패");
	}
}

Actor* Actor::getAttacher()
{
	return m_pAttacher;
}

bool Actor::hasAttacher()
{
	return m_pAttacher != nullptr;
}

bool Actor::hasCleanUpFlag(int _cleanUpFlag)
{
	return (m_iCleanUpFlag & _cleanUpFlag) == _cleanUpFlag;
}

void Actor::addCleanUpFlag(int _cleanUpFlag)
{
	m_iCleanUpFlag |= _cleanUpFlag;
}

void Actor::clearCleanUpFlag()
{
	m_iCleanUpFlag = CF_None;
}

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
		absoluteActorRect.ThicknessRect.origin.x = spawnerCanvasPos.x + (spawnerCanvasSize.width - _relativeRect.
			ThicknessRect.origin.x - _relativeRect.ThicknessRect.size.width);
		absoluteActorRect.ThicknessRect.origin.y = spawnerCanvasPos.y + _relativeRect.ThicknessRect.origin.y;

		absoluteActorRect.BodyRect.origin.x = spawnerCanvasPos.x + (spawnerCanvasSize.width - _relativeRect.BodyRect.
			origin.x - _relativeRect.BodyRect.size.width);
		absoluteActorRect.BodyRect.origin.y = spawnerCanvasPos.y + _relativeRect.BodyRect.origin.y;
	}

	absoluteActorRect.ThicknessRect.size = _relativeRect.ThicknessRect.size;
	absoluteActorRect.BodyRect.size = _relativeRect.BodyRect.size;

	return absoluteActorRect;
}

AIActivity* Actor::getRunningAIActivity()
{
	const AIComponent* pAIComponent = m_Components.get<AIComponent>();

	if (pAIComponent == nullptr)
	{
		return nullptr;
	}

	return pAIComponent->getRunningActivity();
}
