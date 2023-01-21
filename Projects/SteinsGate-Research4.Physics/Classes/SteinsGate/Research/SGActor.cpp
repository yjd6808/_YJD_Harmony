/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:07 AM
 * =====================
 *
 */


#include "SGActor.h"

#include <SteinsGate/Common/Engine/RectPoly.h>

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

	RectPoly poly = RectPoly::createFromCenter(Vec2{ 0, 0 }, thicknessBox.getSize());

	m_pThicknessBox = cocos2d::DrawNode::create();
	m_pThicknessBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pThicknessBox->setPositionY(thicknessBox.RelativeY);
	m_pThicknessBox->setOpacity(125);
	m_pThicknessBox->setContentSize(thicknessBox.getSize());
	m_pThicknessBox->drawPolygon(poly.source(), 4, {}, 1, Color4F::MAGENTA);
	this->addChild(m_pThicknessBox);
}

SGThicknessBox SGActor::getThicknessBox() {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Size size = m_pThicknessBox->getContentSize();
	return { size.width, size.height, m_pThicknessBox->getPositionY() };
}

Rect SGActor::getThicknessBoxRect() {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	return { getPositionReal(), m_pThicknessBox->getContentSize() };
}

cocos2d::Vec2 SGActor::getPositionReal() {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	SGThicknessBox thicknessBox = getThicknessBox();
	Vec2 thicknessOrigin = getPosition();
	thicknessOrigin.x -= thicknessBox.Width / 2.0f;
	thicknessOrigin.y += (thicknessBox.RelativeY - (thicknessBox.Height / 2.0f));
	return thicknessOrigin;
}

cocos2d::Vec2 SGActor::getPositionRealCenter() {
	DebugAssertMessage(m_pThicknessBox, "아직 두께박스가 초기화가 이뤄지지 않았습니다.");
	Vec2 thisPos = getPosition();
	thisPos.y += m_pThicknessBox->getPositionY();
	return thisPos;
}

SGActorSprite* SGActor::getActorSprite() {
	DebugAssertMessage(m_pActorSprite, "액터 스프라이트가 초기화되지 않았습니다.");
	return m_pActorSprite;
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

