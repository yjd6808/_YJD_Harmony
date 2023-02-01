/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:07 AM
 * =====================
 *
 */


#include "SGActor.h"

#include <SteinsGate/Common/Engine/RectPoly.h>
#include <SteinsGate/Common/Engine/SGRectEx.h>
#include <SteinsGate/Common/Engine/SGColor.h>

#include <SteinsGate/Research/SGGlobal.h>
#include <SteinsGate/Research/SGMapLayer.h>


USING_NS_CC;
USING_NS_JC;

SGActor::SGActor(ActorType_t type, int code, SGMapLayer* mapLayer)
	: m_pBelongedMap(mapLayer)
	, m_eActorType(type)
	, m_iCode(code)
	, m_pThicknessBox(nullptr)
	, m_iAllyFlag(0)
	, m_pHitRecorder(nullptr)
	, m_pActorSprite(nullptr)

{
}

SGActor::~SGActor() {
	DeleteSafe(m_pHitRecorder);
}


bool SGActor::init() {
	return true;
}

void SGActor::initThicknessBox(const SGThicknessBox& thicknessBox) {

	// DrawNode는 앵커포인트 신경안쓰고 컨텐츠박스 기준 좌하단부터 그림
	RectPoly poly = RectPoly::createFromLeftBottom(Vec2{ 0, 0 }, thicknessBox.getSize());

	m_pThicknessBox = SGDrawNode::create();
	m_pThicknessBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pThicknessBox->setPositionX(thicknessBox.RelativeX);
	m_pThicknessBox->setPositionY(thicknessBox.RelativeY);
	m_pThicknessBox->setOpacity(125);
	m_pThicknessBox->setContentSize(thicknessBox.getSize());
	m_pThicknessBox->drawPolygon(poly.source(), 4, {}, 1, SGColor4F{ ColorList::Brightgreen_v } );

	this->addChild(m_pThicknessBox);
}

void SGActor::initHitRecorder(int hitPossibleListSize, int alreadyHitMapSize) {
	DebugAssertMessage(m_pHitRecorder == nullptr, "이미 초기화 되어있습니다.");
	m_pHitRecorder = new SGHitRecorder(this, hitPossibleListSize, alreadyHitMapSize);
}

void SGActor::update(float dt) {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->update(dt);


	if (SGGlobal::getInstance()->isThicknessBoxDrawMode())
		m_pThicknessBox->setOpacity(0);
	else
		m_pThicknessBox->setOpacity(125);
}

ActorType_t SGActor::getType() const {
	return m_eActorType;
}

SGActorRect SGActor::getActorRect() const {
	return { getThicknessBoxRect(), getHitBox() };
}

SGThicknessBox SGActor::getThicknessBox() const {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { m_pThicknessBox->getPosition(), m_pThicknessBox->getContentSize() };
}

Rect SGActor::getThicknessBoxRect() const {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { getPositionReal(), m_pThicknessBox->getContentSize() };
}

SGVec2 SGActor::getPositionReal() const {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	thicknessOrigin.y += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOrigin;
}

float SGActor::getPositionRealX() const {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginX = getPositionX();
	thicknessOriginX += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	return thicknessOriginX;
}

float SGActor::getPositionRealY() const {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginY = getPositionY();
	thicknessOriginY += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOriginY;
}

float SGActor::getPositionActorY() const {
	return m_pActorSprite->getPositionY();
}

SGVec2 SGActor::getPositionRealCenter() const {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Vec2 thisPos = getPosition();
	thisPos.x += m_pThicknessBox->getPositionX();
	thisPos.y += m_pThicknessBox->getPositionY();
	return thisPos;
}

float SGActor::getPositionRealCenterX() const {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionX() + m_pThicknessBox->getPositionX();
}

SGVec2 SGActor::getCanvasPositionReal() const {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return this->getPosition() - (m_pActorSprite->getBodyCanvas()->getContentSize() / 2) + m_pActorSprite->getPosition();
}

SGSize SGActor::getCanvasSize() const {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getBodyCanvasSize();
}

SGRect SGActor::getHitBox() const {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	// 위치: 캔버스 좌하단 절대 좌표 + 캔버스 좌하단 기준 스킨 파츠 좌표
	//      캔버스 좌하단 절대 좌표 = 플레이어 Cocos 위치  + 캐릭터 위치 - (캔버스 사이즈 / 2)
	Vec2 canvasPosition = getCanvasPositionReal();
	Vec2 skinPartPos = m_pActorSprite->getBodyPartPosition(); // 캔버스 좌하단 기준 스킨 파츠 좌표
	Rect hitBox{ getCanvasPositionReal() + skinPartPos, m_pActorSprite->getBodyPartSize() };
	return hitBox;
}


SGActorSprite* SGActor::getActorSprite() const {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite;
}

SpriteDirection_t SGActor::getSpriteDirection() const {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getSpriteDirection();
}

int SGActor::getRunningAnimationCode() {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getRunningAnimationCode();
}

SGActorPartAnimation* SGActor::getRunningAnimation() {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getRunningAnimation();
}


void SGActor::setPositionReal(float x, float y) {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();

	x -= thicknessBox.RelativeX;
	y -= thicknessBox.RelativeY;

	x += thicknessBox.Width / 2.0f;
	y += thicknessBox.Height / 2.0f;

	setPosition(x, y);
}

void SGActor::setPositionReal(const SGVec2& v) {
	setPositionReal(v.x, v.y);
}

void SGActor::setPositionRealX(float x) {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	x -= thicknessBox.RelativeX;
	x += thicknessBox.Width / 2.0f;
	setPositionX(x);
}

void SGActor::setPositionRealY(float y) {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	y -= thicknessBox.RelativeY;
	y += thicknessBox.Height / 2.0f;
	setPositionY(y);
}

void SGActor::setPositionRealCenter(float x, float y) {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	setPosition(
		x -= m_pThicknessBox->getPositionX(), 
		y -= m_pThicknessBox->getPositionY()
	);
}

void SGActor::setPositionRealCenter(const SGVec2& v) {
	setPositionRealCenter(v.x, v.y);
}

void SGActor::runAnimation(int animationCode) {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(animationCode);
}

void SGActor::runAnimation(int animationCode, int startFrameIndexInAnimation) {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(animationCode, startFrameIndexInAnimation);
}

void SGActor::pauseAnimation(float delay) {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->pauseAnimation(delay);
}

void SGActor::runFrameEvent(FrameEventType_t frameEventType, int frameEventId) {
	m_pBelongedMap->runFrameEvent(this, frameEventType, frameEventId);
}

void SGActor::setSpriteDirection(SpriteDirection_t direction) {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setSpriteDirection(direction);
}


void SGActor::setForwardDirection() {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setForwardDirection();
}

void SGActor::setBackwardDirection() {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setBackwardDirection();
}

bool SGActor::isCollide(SGActor* other, Out_ SpriteDirection_t& otherHitDirection, Out_ SGRect& hitRect) {
	SGRect myThick = getThicknessBoxRect();
	SGRect otherBox = other->getThicknessBoxRect();

	if (!SGRectEx::intersectY(myThick, otherBox)) {
		return false;
	}

	SGRect myHit = getHitBox();
	SGRect targetHit = other->getHitBox();

	if (SGRectEx::intersect(myHit, targetHit, hitRect)) {
		otherHitDirection = targetHit.getMidX() > myHit.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		return true;
	}

	return false;
}

bool SGActor::isCollide(const SGActorRect& otherRect, SpriteDirection_t& otherHitDirection, SGRect& hitRect) {
	SGRect myThick = getThicknessBoxRect();

	if (!SGRectEx::intersectY(myThick, otherRect.ThicknessRect)) {
		return false;
	}

	SGRect myHit = getHitBox();

	if (SGRectEx::intersect(myHit, otherRect.BodyRect, hitRect)) {
		// otherHitDirection = hitRect.getMidX() < otherRect.BodyRect.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		otherHitDirection = otherRect.BodyRect.getMidX() > myHit.getMidX() ? SpriteDirection::Left : SpriteDirection::Right;
		return true;
	}

	return false;
}

bool SGActor::isCollide(const SGActorRect& otherRect) {
	SGRect myThick = getThicknessBoxRect();

	if (!SGRectEx::intersectY(myThick, otherRect.ThicknessRect)) {
		return false;
	}

	SGRect myHit = getHitBox();
	return myHit.intersectsRect(otherRect.BodyRect);
}

bool SGActor::isOnTheGround() {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getPositionY() == 0;
}

SGActorRect SGActor::convertAbsoluteActorRect(SGActor* stdActor, const SGActorRect& relativeRect) {
	SGActorRect absoluteActorRect;
	SGSize spawnerCanvsSize = stdActor->getCanvasSize();
	SGVec2 spawnerCanvasPos = stdActor->getCanvasPositionReal();

	if (stdActor->getSpriteDirection() == SpriteDirection::Right) {
		absoluteActorRect.ThicknessRect.origin.x = spawnerCanvasPos.x + relativeRect.ThicknessRect.origin.x;
		absoluteActorRect.ThicknessRect.origin.y = spawnerCanvasPos.y + relativeRect.ThicknessRect.origin.y;

		absoluteActorRect.BodyRect.origin.x = spawnerCanvasPos.x + relativeRect.BodyRect.origin.x;
		absoluteActorRect.BodyRect.origin.y = spawnerCanvasPos.y + relativeRect.BodyRect.origin.y;
	}
	else {
		absoluteActorRect.ThicknessRect.origin.x = spawnerCanvasPos.x + (spawnerCanvsSize.width - relativeRect.ThicknessRect.origin.x - relativeRect.ThicknessRect.size.width);
		absoluteActorRect.ThicknessRect.origin.y = spawnerCanvasPos.y + relativeRect.ThicknessRect.origin.y;

		absoluteActorRect.BodyRect.origin.x = spawnerCanvasPos.x + (spawnerCanvsSize.width - relativeRect.BodyRect.origin.x - relativeRect.ThicknessRect.size.width);
		absoluteActorRect.BodyRect.origin.y = spawnerCanvasPos.y + relativeRect.BodyRect.origin.y;
	}

	absoluteActorRect.ThicknessRect.size = relativeRect.ThicknessRect.size;
	absoluteActorRect.BodyRect.size = relativeRect.BodyRect.size;

	return absoluteActorRect;
}

