/*
 * 작성자: 윤정도
 * 생성일: 1/15/2023 10:39:28 PM
 * =====================
 *
 */


#include "ColliderTarget.h"

using namespace cocos2d;

struct TesterTargetInfo
{
	int TypeId;
	cocos2d::Size FrameSize;
};

ColliderTarget* ColliderTarget::create() {
	ColliderTarget* pTarget = new ColliderTarget{ };

	if (pTarget && pTarget->init()) {
		pTarget->autorelease();
		return pTarget;
	}

	CC_SAFE_DELETE(pTarget);
	return pTarget;
}


bool ColliderTarget::init() {

	if (!Sprite::init())
		return false;

	// info->getFrameSize();

	float fFrameWidth = 300;
	float fFrameHeight= 300;

	float fWidth = 100;
	float fHeight = 100;


	m_pCanvas = Node::create();
	m_pCanvas->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pCanvas->setPosition(-fFrameWidth / 2, -fFrameHeight / 2);
	m_pCanvas->setContentSize({ fFrameWidth , fFrameHeight });
	this->addChild(m_pCanvas);

	m_pSkinPart = Sprite::create("rect.png");
	m_pSkinPart->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pSkinPart->setContentSize({ fWidth, fHeight });
	m_pSkinPart->setCascadeOpacityEnabled(false);
	m_pSkinPart->setOpacity(80.0f);
	m_pSkinPart->setPosition(100, 100);
	m_pCanvas->addChild(m_pSkinPart);

	Vec2 polygon[4];
	polygon[0] = Vec2(0.0f, fHeight);
	polygon[1] = Vec2(fWidth, fHeight);
	polygon[2] = Vec2(fWidth, 0.0f);
	polygon[3] = Vec2(0.0f, 0.0f);

	m_pSkinBoundingBox = DrawNode::create();
	m_pSkinBoundingBox->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pSkinBoundingBox->setContentSize({ fWidth, fHeight });
	m_pSkinBoundingBox->drawPolygon(polygon, 4, {}, 1, Color4F::WHITE);

	m_pSkinPart->addChild(m_pSkinBoundingBox);
	

	return true;
}

