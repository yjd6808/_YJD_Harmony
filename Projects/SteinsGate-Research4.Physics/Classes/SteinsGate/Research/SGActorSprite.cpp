/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:44 PM
 * =====================
 *
 */


#include "SGActorSprite.h"

USING_NS_CC;
USING_NS_JC;

SGActorSprite::SGActorSprite(const SGActorSpriteDataPtr& actorData)
	: m_spActorData(actorData)
	, m_pCanvas(nullptr)
	, m_Parts(10)
{
}

SGActorSprite* SGActorSprite::create(const SGActorSpriteDataPtr& actorData) {
	SGActorSprite* pSprite = new SGActorSprite(actorData);

	if (pSprite && pSprite->init()) {
		pSprite->autorelease();
		return pSprite;
	}

	DeleteSafe(pSprite);
	return pSprite;
}

bool SGActorSprite::init() {
	if (!Sprite::init())
		return false;

	m_pCanvas = Sprite::create("rect.png");
	m_pCanvas->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pCanvas->setCascadeOpacityEnabled(false);
	m_pCanvas->setOpacity(0);
	this->addChild(m_pCanvas, 0);

	m_spActorData->Parts.Sort([](SGActorPartSpriteData& lhs, SGActorPartSpriteData& rhs) { return lhs.ZOrder < rhs.ZOrder; });


	return true;
}

void SGActorSprite::update(float delta) {

}
