/*
 * 작성자: 윤정도
 * 생성일: 1/16/2023 7:37:38 AM
 * =====================
 *
 */


#include "BulletTarget.h"

#include "PackLoader.h"
#include "SteinsGate/Common/Engine/RectPoly.h"

using namespace cocos2d;

struct TesterTargetInfo
{
	int TypeId;
	cocos2d::Size FrameSize;
};

BulletTarget* BulletTarget::create(int projectileId) {
	BulletTarget* pTarget = new BulletTarget{  };

	if (pTarget && pTarget->init(projectileId)) {
		pTarget->autorelease();
		return pTarget;
	}

	CC_SAFE_DELETE(pTarget);
	return pTarget;
}


bool BulletTarget::init(int projectileId) {

	if (!Sprite::init())
		return false;

	// 파츠 들을 생성 한 후 캔버스를 생성하도록 한다.
	// 파츠들의 크기가 생성중 변경될 수 있기땜이다.

	FrameTexturePtr ptr = PackLoader::GetInstance()->CreateFrameTexture(11, 0, 0);

	m_pSkinPart = Sprite::createWithTexture(ptr->getTexture());
	m_pSkinPart->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	if (projectileId == 2 || projectileId == 3) {
		m_pSkinPart->setRotation(30.0f);
	}

	auto box = m_pSkinPart->getBoundingBox();
	m_pSkinPart->setPositionY(-box.origin.y); // 회전으로인한 보정

	// 프로젝틸은 프레임사이즈, 크기 동일하다고 가정
	float fWidth = box.size.width;
	float fHeight = box.size.height;

	m_pCanvas = Node::create();
	m_pCanvas->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pCanvas->setPosition(-fWidth / 2, -fHeight / 2);
	m_pCanvas->setContentSize({ fWidth , fHeight });
	this->addChild(m_pCanvas);
	m_pCanvas->addChild(m_pSkinPart);

	
	RectPoly poly = RectPoly::createFromLeftBottom({}, box.size);

	m_pSkinBoundingBox = DrawNode::create();
	m_pSkinBoundingBox->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pSkinBoundingBox->setContentSize({ box.size.width, box.size.height });
	m_pSkinBoundingBox->drawPolygon(poly.source(), 4, {}, 1, Color4F::WHITE);
	m_pCanvas->addChild(m_pSkinBoundingBox);
	return true;
}

