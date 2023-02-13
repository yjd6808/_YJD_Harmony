/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:07 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGActor.h"

#include <SteinsGate/Common/RectPoly.h>
#include <SteinsGate/Common/SGRectEx.h>
#include <SteinsGate/Common/SGColor.h>

#include <SteinsGate/Client/SGGlobal.h>
#include <SteinsGate/Client/SGMapLayer.h>


USING_NS_CC;
USING_NS_JC;

SGActor::SGActor(ActorType_t type, int code)
	: m_eActorType(type)
	, m_iActorId(InvalidValue_v)
	, m_iCode(code)
	, m_pThicknessBox(nullptr)
	, m_iAllyFlag(0)
	, m_pHitRecorder(nullptr)
	, m_pActorSprite(nullptr)
	, m_pMapLayer(nullptr)
	, m_pListener(nullptr)
	, m_bCleanUp(false)
	, m_vAttches(4)
	, m_pAttacher(nullptr)
{}

SGActor::~SGActor() {
	DeleteSafe(m_pHitRecorder);
	DeleteSafe(m_pListener);
}


bool SGActor::initVariables() {

	// 이거 2개만 초기화 시키면 댈듯?
	m_iActorId = InvalidValue_v;
	m_pMapLayer = nullptr;
	m_vAttches.Clear();
	m_pAttacher = nullptr;
	m_bCleanUp = false;

	// m_eActorType = ;
	// m_iCode = ;
	// m_pThicknessBox = nullptr;
	// m_iAllyFlag = 0;
	// m_pHitRecorder = nullptr;
	// m_pActorSprite = nullptr;
	// m_pListener = nullptr;

	return true;
}

void SGActor::initThicknessBox(const SGThicknessBox& thicknessBox) {

	// DrawNode는 앵커포인트 신경안쓰고 컨텐츠박스 기준 좌하단부터 그림
	RectPoly poly = RectPoly::createFromLeftBottom(Vec2{ 0, 0 }, thicknessBox.getSize());

	if (m_pThicknessBox == nullptr) {
		m_pThicknessBox = SGDrawNode::create();
		m_pThicknessBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pThicknessBox->setOpacity(125);
		m_pThicknessBox->drawPolygon(poly.source(), 4, {}, 1, SGColor4F{ ColorList::Brightgreen_v });
		this->addChild(m_pThicknessBox);
	}
	m_pThicknessBox->setPositionX(thicknessBox.RelativeX);
	m_pThicknessBox->setPositionY(thicknessBox.RelativeY);
	m_pThicknessBox->setContentSize(thicknessBox.getSize());
	
}

void SGActor::initHitRecorder(int hitPossibleListSize, int alreadyHitMapSize) {
	DebugAssertMsg(m_pHitRecorder == nullptr, "이미 히트 레코더가 초기화 되어있습니다.");
	m_pHitRecorder = dbg_new SGHitRecorder(this, hitPossibleListSize, alreadyHitMapSize);
}

void SGActor::update(float dt) {
	DebugAssertMsg(m_bCleanUp == false, "다음 프레임에 풀로 복귀 예정중인 객체입니다.");
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	DebugAssertMsg(m_pMapLayer, "맵 레이어가 세팅되지 않았습니다.");

	m_pActorSprite->update(dt);


	if (SGGlobal::get()->isThicknessBoxDrawMode())
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
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { m_pThicknessBox->getPosition(), m_pThicknessBox->getContentSize() };
}

Rect SGActor::getThicknessBoxRect() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { getPositionReal(), m_pThicknessBox->getContentSize() };
}

SGVec2 SGActor::getPositionReal() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	thicknessOrigin.y += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOrigin;
}

float SGActor::getPositionRealX() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginX = getPositionX();
	thicknessOriginX += (thicknessBox.RelativeX - (thicknessBox.Width / 2.0f));
	return thicknessOriginX;
}

float SGActor::getPositionRealY() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	float thicknessOriginY = getPositionY();
	thicknessOriginY += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOriginY;
}

float SGActor::getPositionActorY() const {
	return m_pActorSprite->getPositionY();
}

SGVec2 SGActor::getPositionRealCenter() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Vec2 thisPos = getPosition();
	thisPos.x += m_pThicknessBox->getPositionX();
	thisPos.y += m_pThicknessBox->getPositionY();
	return thisPos;
}

float SGActor::getPositionRealCenterX() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionX() + m_pThicknessBox->getPositionX();
}

float SGActor::getPositionRealCenterY() const {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return getPositionY() + m_pThicknessBox->getPositionY();
}

SGVec2 SGActor::getCanvasPositionReal() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return this->getPosition() - (m_pActorSprite->getBodyCanvas()->getContentSize() / 2) + m_pActorSprite->getPosition();
}

SGSize SGActor::getCanvasSize() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getBodyCanvasSize();
}

SGRect SGActor::getHitBox() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	// 위치: 캔버스 좌하단 절대 좌표 + 캔버스 좌하단 기준 스킨 파츠 좌표
	//      캔버스 좌하단 절대 좌표 = 플레이어 Cocos 위치  + 캐릭터 위치 - (캔버스 사이즈 / 2)
	Vec2 canvasPosition = getCanvasPositionReal();
	Vec2 skinPartPos = m_pActorSprite->getBodyPartPosition(); // 캔버스 좌하단 기준 스킨 파츠 좌표
	Rect hitBox{ getCanvasPositionReal() + skinPartPos, m_pActorSprite->getBodyPartSize() };
	return hitBox;
}


SGActorSprite* SGActor::getActorSprite() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite;
}

SpriteDirection_t SGActor::getSpriteDirection() const {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getSpriteDirection();
}

int SGActor::getRunningAnimationCode() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getRunningAnimationCode();
}

SGActorPartAnimation* SGActor::getRunningAnimation() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite->getRunningAnimation();
}


void SGActor::setPositionReal(float x, float y) {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
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
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	x -= thicknessBox.RelativeX;
	x += thicknessBox.Width / 2.0f;
	setPositionX(x);
}

void SGActor::setPositionRealY(float y) {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	y -= thicknessBox.RelativeY;
	y += thicknessBox.Height / 2.0f;
	setPositionY(y);
}

void SGActor::setPositionRealCenter(float x, float y) {
	DebugAssertMsg(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	setPosition(
		x -= m_pThicknessBox->getPositionX(), 
		y -= m_pThicknessBox->getPositionY()
	);
}

void SGActor::setPositionRealCenter(const SGVec2& v) {
	setPositionRealCenter(v.x, v.y);
}

void SGActor::setMapLayer(SGMapLayer* mapLayer) {
	m_pMapLayer = mapLayer;
}

void SGActor::setActorId(int id) {

	// 청소되지 않은 액터에 할당을 시도할려는 경우를 막아야한다.
	if (!m_bCleanUp) {
		DebugAssertMsg(m_iActorId == InvalidValue_v, "이미 ID값이 할당되어 있습니다.");
	}

	m_iActorId = id;
}

void SGActor::runAnimation(int animationCode) {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(animationCode);
}

void SGActor::runAnimation(int animationCode, int startFrameIndexInAnimation) {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(animationCode, startFrameIndexInAnimation);
}

void SGActor::pauseAnimation(float delay) {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->pauseAnimation(delay);
}

void SGActor::runFrameEvent(FrameEventType_t frameEventType, int frameEventId) {
	m_pMapLayer->runFrameEvent(this, frameEventType, frameEventId);
}

void SGActor::setSpriteDirection(SpriteDirection_t direction) {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setSpriteDirection(direction);
}


void SGActor::setForwardDirection() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->setForwardDirection();
}

void SGActor::setBackwardDirection() {
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
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
	DebugAssertMsg(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getPositionY() <= 0;
}

void SGActor::cleanUpNext() {
	DebugAssertMsg(m_pMapLayer, "소속된 맵이 존재하지 않습니다.");
	SGActorBox::get()->registerCleanUp(this);
	m_bCleanUp = true;
}

void SGActor::attach(SGActor* actor) {
	DebugAssertMsg(actor->hasAttacher() == false, "어태치 할려는 액터가 이미 다른 누군가에게 어태치 되어 있습니다.");
	DebugAssertMsg(m_vAttches.Exist(actor) == false, "동일한 액터에 대해서 연속 어태치 할 수 없습니다.");

	m_vAttches.PushBack(actor);

	actor->m_pAttacher = this;
	actor->retain();
}

void SGActor::detach(SGActor* actor) {
	DebugAssertMsg(m_vAttches.Exist(actor), "디태치 할려는 액터가 어태치되어 있지 않습니다.");
	if (m_vAttches.Remove(actor) == false) {
		DebugAssertMsg(false, "디태치 실패");
	}
}

SGActor* SGActor::getAttacher() {
	return m_pAttacher;
}

bool SGActor::hasAttacher() {
	return m_pAttacher != nullptr;
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

		absoluteActorRect.BodyRect.origin.x = spawnerCanvasPos.x + (spawnerCanvsSize.width - relativeRect.BodyRect.origin.x - relativeRect.BodyRect.size.width);
		absoluteActorRect.BodyRect.origin.y = spawnerCanvasPos.y + relativeRect.BodyRect.origin.y;
	}

	absoluteActorRect.ThicknessRect.size = relativeRect.ThicknessRect.size;
	absoluteActorRect.BodyRect.size = relativeRect.BodyRect.size;

	return absoluteActorRect;
}

