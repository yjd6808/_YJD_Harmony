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
	, m_vAttches(4)
	, m_pAttacher(nullptr)
{}

Actor::~Actor() {
	JCORE_DELETE_SAFE(m_pHitRecorder);
	m_Listeners.deleteAll();
}


bool Actor::initVariables() {

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

bool Actor::addListener(IActorListener* listener) {
	return m_Listeners.add(listener);
}

bool Actor::hasListener(IActorListener::Type type) {
	return m_Listeners.has(type);
}

IActorListener* Actor::getListener(IActorListener::Type type) {
	return m_Listeners.get(type);
}

void Actor::initThicknessBox(const ThicknessBox& thicknessBox) {

	// DrawNode는 앵커포인트 신경안쓰고 컨텐츠박스 기준 좌하단부터 그림
	RectPoly poly = RectPoly::createFromLeftBottom(0, 0, thicknessBox.Width, thicknessBox.Height);

	if (m_pThicknessBox == nullptr) {
		m_pThicknessBox = SGDrawNode::create();
		m_pThicknessBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pThicknessBox->setOpacity(125);
		m_pThicknessBox->drawPolygon(poly.source(), 4, {}, 1, SGColor4F{ ColorList::Brightgreen_v });
		this->addChild(m_pThicknessBox);
	}
	m_pThicknessBox->setPositionX(thicknessBox.RelativeX);
	m_pThicknessBox->setPositionY(thicknessBox.RelativeY);
	m_pThicknessBox->setContentSize(thicknessBox.Width, thicknessBox.Height);
	
}

void Actor::initHitRecorder(int hitPossibleListSize /* = 16 */, int alreadyHitMapSize /* = 32 */, Actor* owner /*= nullptr */) {
	Actor* pOwner = owner;

	if (pOwner == nullptr)
		pOwner = this;

	if (m_pHitRecorder == nullptr)
		m_pHitRecorder = dbg_new HitRecorder(pOwner, this, hitPossibleListSize, alreadyHitMapSize);
}

void Actor::hit(const HitInfo& hitInfo) {
	PhysicsComponent* pPhysicsComponent = getComponent<PhysicsComponent>();

	if (pPhysicsComponent) {
		pPhysicsComponent->hit(hitInfo);
	}

	getListenerCollection().onHit(hitInfo);
}

void Actor::update(float dt) {
	DebugAssertMsg(m_bCleanUp == false, "다음 프레임에 풀로 복귀 예정중인 객체입니다.");
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	DebugAssertMsg(m_pMapLayer, "맵 레이어가 세팅되지 않았습니다.");

	m_pActorSprite->update(dt);
	m_Listeners.onUpdate(dt);
	m_Components.onUpdate(dt);


	if (Core::Contents.Global->DrawThicknessBox)
		m_pThicknessBox->setOpacity(0);
	else
		m_pThicknessBox->setOpacity(125);
}

void Actor::addComponent(IComponent* componenet) {
	m_Components.add(componenet);
}

bool Actor::hasComponent(IComponent::Type type) const {
	return m_Components.has(type);
}

const char* Actor::getTypeName() const {
	return ActorType::Name[getType()];
}

ActorRect Actor::getActorRect() const {
	return { getThicknessBoxRect(), getHitBox() };
}

ThicknessBox Actor::getThicknessBox() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");

	SGVec2 pos = m_pThicknessBox->getPosition();
	SGSize size = m_pThicknessBox->getContentSize();

	return { pos.x, pos.y, size.width, size.height };
}

SGDrawNode* Actor::getThicknessBoxNode() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return m_pThicknessBox;
}

Rect Actor::getThicknessBoxRect() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { getPositionReal(), m_pThicknessBox->getContentSize() };
}

SGVec2 Actor::getPositionReal() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	thicknessOrigin.y += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOrigin;
}

float Actor::getPositionRealX() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginX = getPositionX();
	thicknessOriginX += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	return thicknessOriginX;
}

float Actor::getPositionRealY() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginY = getPositionY();
	thicknessOriginY += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOriginY;
}

float Actor::getPositionActorY() const {
	return m_pActorSprite->getPositionY();
}

SGVec2 Actor::getPositionRealCenter() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Vec2 thisPos = getPosition();
	thisPos.x += m_pThicknessBox->getPositionX();
	thisPos.y += m_pThicknessBox->getPositionY();
	return thisPos;
}

float Actor::getPositionRealCenterX() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionX() + m_pThicknessBox->getPositionX();
}

float Actor::getPositionRealCenterY() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionY() + m_pThicknessBox->getPositionY();
}

SGVec2 Actor::getCanvasPositionReal() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	SGSize contentSize = m_pActorSprite->getBodyCanvas()->getContentSize();
	SGVec2 canvasRealPos = this->getPosition() - (contentSize / 2) + m_pActorSprite->getPosition();
	return canvasRealPos;
}

SGSize Actor::getCanvasSize() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getBodyCanvasSize();
}

SGRect Actor::getHitBox() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	// 위치: 캔버스 좌하단 절대 좌표 + 캔버스 좌하단 기준 스킨 파츠 좌표
	//      캔버스 좌하단 절대 좌표 = 플레이어 Cocos 위치  + 캐릭터 위치 - (캔버스 사이즈 / 2)
	Vec2 canvasPosition = getCanvasPositionReal();
	Vec2 skinPartPos = m_pActorSprite->getBodyPartPosition(); // 캔버스 좌하단 기준 스킨 파츠 좌표
	Rect hitBox{ getCanvasPositionReal() + skinPartPos, m_pActorSprite->getBodyPartSize() };
	return hitBox;
}


ActorSprite* Actor::getActorSprite() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite;
}

SpriteDirection_t Actor::getSpriteDirection() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getSpriteDirection();
}

int Actor::getRunningAnimationCode() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getRunningAnimationCode();
}

ActorPartAnimation* Actor::getRunningAnimation() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getRunningAnimation();
}


void Actor::setPositionReal(float x, float y) {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();

	x -= thicknessBox.RelativeX;
	y -= thicknessBox.RelativeY;

	x += thicknessBox.Width / 2.0f;
	y += thicknessBox.Height / 2.0f;

	setPosition(x, y);
}

void Actor::setPositionReal(const SGVec2& v) {
	setPositionReal(v.x, v.y);
}

void Actor::setPositionRealX(float x) {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	x -= thicknessBox.RelativeX;
	x += thicknessBox.Width / 2.0f;
	setPositionX(x);
}

void Actor::setPositionRealY(float y) {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	ThicknessBox thicknessBox = getThicknessBox();
	y -= thicknessBox.RelativeY;
	y += thicknessBox.Height / 2.0f;
	setPositionY(y);
}

void Actor::setPositionRealCenter(float x, float y) {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	setPosition(
		x -= m_pThicknessBox->getPositionX(), 
		y -= m_pThicknessBox->getPositionY()
	);
}

void Actor::setPositionRealCenter(const SGVec2& v) {
	setPositionRealCenter(v.x, v.y);
}

void Actor::setMapLayer(MapLayer* mapLayer) {
	m_pMapLayer = mapLayer;
}

void Actor::setActorId(int id) {

	// 청소되지 않은 액터에 할당을 시도할려는 경우를 막아야한다.
	if (!m_bCleanUp) {
		DebugAssertMsg(m_iActorId == InvalidValue_v, "이미 ID값이 할당되어 있습니다.");
	}

	m_iActorId = id;
}

void Actor::runAnimation(int animationCode) {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(animationCode);
}

void Actor::runAnimation(int animationCode, int startFrameIndexInAnimation) {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(animationCode, startFrameIndexInAnimation);
}

void Actor::pauseAnimation(float delay) {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->pauseAnimation(delay);
}

void Actor::runFrameEventSpawn(FrameEventSpawnType_t spawnType,  int spawnCode) {
	switch (spawnType) {
	case FrameEventSpawnType::Projectile:
		Core::Contents.ActorManager->createProejctileOnMap(this, spawnCode);
		break;
	case FrameEventSpawnType::AttackBox:
		DebugAssertMsg(false, "미구현");
		break;
	default: DebugAssert(false);
	}
}

void Actor::runFrameEvent(int frameEventCode) {
	if (frameEventCode == InvalidValue_v)
		return;

	FrameEvent* pFrameEvent = Core::DataManager->getFrameEvent(getType(), frameEventCode);

	if (pFrameEvent == nullptr)
		return;

	runFrameEvent(pFrameEvent);
}


void Actor::runFrameEvent(FrameEvent* frameEvent) {
	switch (frameEvent->Type) {
	case FrameEventType::Spawn: {
		const FrameEventSpawn* pSpawn = dynamic_cast<FrameEventSpawn*>(frameEvent);

		if (pSpawn == nullptr) {
			DebugAssert(false);
			break;
		}
		runFrameEventSpawn(pSpawn->SpawnType, pSpawn->SpawnCode);
		break;
	}
	case FrameEventType::AttackBoxInstant: {
		const FrameEventAttackBoxInstant* pAttackBoxInstant = dynamic_cast<FrameEventAttackBoxInstant*>(frameEvent);

		if (pAttackBoxInstant == nullptr) {
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

void Actor::onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	m_Listeners.onFrameBegin(animation, texture);
	runFrameEvent(animation->getRunningFrameEventCode());

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity) {
		pRunningActivity->onFrameBegin(animation, texture);
	}
}

void Actor::onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	m_Listeners.onFrameEnd(animation, texture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity) {
		pRunningActivity->onFrameEnd(animation, texture);
	}
}
void Actor::onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) {
	m_Listeners.onAnimationBegin(animation, texture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity) {
		pRunningActivity->onAnimationBegin(animation, texture);
	}
}
void Actor::onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) {
	m_Listeners.onAnimationEnd(animation, texture);

	AIActivity* pRunningActivity = getRunningAIActivity();

	if (pRunningActivity) {
		pRunningActivity->onAnimationEnd(animation, texture);
	}
}



void Actor::setSpriteDirection(SpriteDirection_t direction) {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setSpriteDirection(direction);
}


void Actor::setForwardDirection() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setForwardDirection();
}

void Actor::setBackwardDirection() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setBackwardDirection();
}

bool Actor::isCollide(Actor* other, JCORE_OUT SpriteDirection_t& otherHitDirection, JCORE_OUT SGRect& hitRect) {
	SGRect myThick = getThicknessBoxRect();
	SGRect otherBox = other->getThicknessBoxRect();

	if (!RectEx::intersectY(myThick, otherBox)) {
		return false;
	}

	SGRect myHit = getHitBox();
	SGRect targetHit = other->getHitBox();

	if (RectEx::intersect(myHit, targetHit, hitRect)) {
		otherHitDirection = targetHit.getMidX() > myHit.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		return true;
	}

	return false;
}

bool Actor::isCollide(const ActorRect& otherRect, SpriteDirection_t& otherHitDirection, SGRect& hitRect) {
	SGRect myThick = getThicknessBoxRect();

	if (!RectEx::intersectY(myThick, otherRect.ThicknessRect)) {
		return false;
	}

	SGRect myHit = getHitBox();

	if (RectEx::intersect(myHit, otherRect.BodyRect, hitRect)) {
		otherHitDirection = otherRect.BodyRect.getMidX() > myHit.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		return true;
	}

	return false;
}

bool Actor::isCollide(const ActorRect& otherRect) {
	SGRect myThick = getThicknessBoxRect();

	if (!RectEx::intersectY(myThick, otherRect.ThicknessRect)) {
		return false;
	}

	SGRect myHit = getHitBox();
	return myHit.intersectsRect(otherRect.BodyRect);
}

bool Actor::isOnTheGround() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getPositionY() <= 0;
}

void Actor::releaseActorSprite() {
	if (!m_pActorSprite)
		return;

	removeChild(m_pActorSprite);
	m_pActorSprite = nullptr;
}

void Actor::cleanUpAtNextFrame() {
	DebugAssertMsg(m_pMapLayer, "소속된 맵이 존재하지 않습니다.");
	Core::Contents.ActorManager->cleanUpAtNextFrame(this);
	m_bCleanUp = true;
}

void Actor::cleanUp() {
	m_bCleanUp = true;
	m_pMapLayer = nullptr;
	Core::Contents.ActorManager->cleanUp(this);
}

void Actor::attach(Actor* actor) {
	DebugAssertMsg(actor->hasAttacher() == false, "어태치 할려는 액터가 이미 다른 누군가에게 어태치 되어 있습니다.");
	DebugAssertMsg(m_vAttches.Exist(actor) == false, "동일한 액터에 대해서 연속 어태치 할 수 없습니다.");

	m_vAttches.PushBack(actor);

	actor->m_pAttacher = this;
	actor->retain();
}

void Actor::detach(Actor* actor) {
	DebugAssertMsg(m_vAttches.Exist(actor), "디태치 할려는 액터가 어태치되어 있지 않습니다.");
	if (m_vAttches.Remove(actor) == false) {
		DebugAssertMsg(false, "디태치 실패");
	}
}

Actor* Actor::getAttacher() {
	return m_pAttacher;
}

bool Actor::hasAttacher() {
	return m_pAttacher != nullptr;
}

bool Actor::hasCleanUpFlag(int cleanUpFlag) {
	return (m_iCleanUpFlag & cleanUpFlag) == cleanUpFlag;
}

void Actor::addCleanUpFlag(int cleanUpFlag) {
	m_iCleanUpFlag |= cleanUpFlag;
}

void Actor::clearCleanUpFlag() {
	m_iCleanUpFlag = CF_None;
}

ActorRect Actor::convertAbsoluteActorRect(Actor* stdActor, const ActorRect& relativeRect) {
	ActorRect absoluteActorRect;
	const SGSize spawnerCanvasSize = stdActor->getCanvasSize();
	const SGVec2 spawnerCanvasPos = stdActor->getCanvasPositionReal();

	if (stdActor->getSpriteDirection() == SpriteDirection::Right) {
		absoluteActorRect.ThicknessRect.origin.x = spawnerCanvasPos.x + relativeRect.ThicknessRect.origin.x;
		absoluteActorRect.ThicknessRect.origin.y = spawnerCanvasPos.y + relativeRect.ThicknessRect.origin.y;

		absoluteActorRect.BodyRect.origin.x = spawnerCanvasPos.x + relativeRect.BodyRect.origin.x;
		absoluteActorRect.BodyRect.origin.y = spawnerCanvasPos.y + relativeRect.BodyRect.origin.y;
	} else {
		absoluteActorRect.ThicknessRect.origin.x = spawnerCanvasPos.x + (spawnerCanvasSize.width - relativeRect.ThicknessRect.origin.x - relativeRect.ThicknessRect.size.width);
		absoluteActorRect.ThicknessRect.origin.y = spawnerCanvasPos.y + relativeRect.ThicknessRect.origin.y;

		absoluteActorRect.BodyRect.origin.x = spawnerCanvasPos.x + (spawnerCanvasSize.width - relativeRect.BodyRect.origin.x - relativeRect.BodyRect.size.width);
		absoluteActorRect.BodyRect.origin.y = spawnerCanvasPos.y + relativeRect.BodyRect.origin.y;
	}

	absoluteActorRect.ThicknessRect.size = relativeRect.ThicknessRect.size;
	absoluteActorRect.BodyRect.size = relativeRect.BodyRect.size;

	return absoluteActorRect;
}

AIActivity* Actor::getRunningAIActivity() {
	const AIComponent* pAIComponent = m_Components.get<AIComponent>();

	if (pAIComponent == nullptr) {
		return nullptr;
	}

	return pAIComponent->getRunningActivity();
}

