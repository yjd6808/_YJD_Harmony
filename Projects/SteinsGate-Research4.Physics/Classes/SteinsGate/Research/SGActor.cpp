/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:07 AM
 * =====================
 *
 */


#include "SGActor.h"

#include <SteinsGate/Common/Engine/RectPoly.h>
#include <SteinsGate/Research/SGGlobal.h>

USING_NS_CC;
USING_NS_JC;

SGActor::SGActor(ActorType_t type, int code)
	: m_eActorType(type)
	, m_iCode(code)
	, m_pThicknessBox(nullptr)

{
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
	m_pThicknessBox->drawPolygon(poly.source(), 4, {}, 1, Color4F::MAGENTA);

	this->addChild(m_pThicknessBox);
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
	thicknessOrigin.x -= thicknessBox.Width / 2.0f;
	thicknessOrigin.y += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOrigin;
}

SGVec2 SGActor::getPositionRealCenter() const {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Vec2 thisPos = getPosition();
	thisPos.y += m_pThicknessBox->getPositionY();
	return thisPos;
}

SGVec2 SGActor::getCanvasPositionReal() const {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return this->getPosition() - (m_pActorSprite->getBodyCanvas()->getContentSize() / 2) + m_pActorSprite->getPosition();
}

SGSize SGActor::getCanvasSize() const {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	return m_pActorSprite->getBodyCanvasSize();
}

SGRect SGActor::getHitbox() const {
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


void SGActor::setPositionReal(float x, float y) {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	x += thicknessBox.Width / 2.0f;
	y -= thicknessBox.RelativeY;
	y += thicknessBox.Height / 2.0f;

	setPosition(x, y);
}

void SGActor::setPositionRealCenter(float x, float y) {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	setPosition(x, y -= m_pThicknessBox->getPositionY());
}

void SGActor::runAnimation(int animationCode) {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 없습니다.");
	m_pActorSprite->runAnimation(animationCode);
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

