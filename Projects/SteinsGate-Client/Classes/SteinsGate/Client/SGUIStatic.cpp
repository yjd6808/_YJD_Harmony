/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:43 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIStatic.h"

USING_NS_CC;
USING_NS_JC;

SGUIStatic::SGUIStatic(SGUIGroup* parent, SGUIStaticInfo* staticInfo)
	: SGUIElement(parent, staticInfo)
	, m_bVisible(false)
	, m_pInfo(staticInfo)
	, m_pDebugTexture{}
	, m_pDebugSprite{}
{}

SGUIStatic::~SGUIStatic() {
	CC_SAFE_RELEASE(m_pDebugTexture);
}

SGUIStatic* SGUIStatic::create(SGUIGroup* parent, SGUIStaticInfo* staticInfo) {
	SGUIStatic* pStatic = dbg_new SGUIStatic(parent, staticInfo);
	pStatic->init();
	pStatic->autorelease();
	return pStatic;
}

bool SGUIStatic::init() {
	setContentSize(m_pInfo->Size);
	return true;
}

void SGUIStatic::load() {
	if (m_bLoaded)
		return;
	m_pDebugTexture = CoreGlobal_v->getDefaultFrameTexture();
	m_pDebugTexture->retain();

	const Size spriteSize = m_pDebugTexture->getSize();
	const float fScaleX = m_pInfo->Size.width / spriteSize.width;
	const float fScaleY = m_pInfo->Size.height / spriteSize.height;

	m_pDebugSprite = Sprite::create();
	m_pDebugSprite->initWithTexture(m_pDebugTexture->getTexture());
	m_pDebugSprite->setAnchorPoint(Vec2::ZERO);
	m_pDebugSprite->setScale(fScaleX, fScaleY);
	m_pDebugSprite->setOpacity(125);
	m_pDebugSprite->setColor(SGColorList::Africanviolet_v);
	m_pDebugSprite->setVisible(m_bVisible);

	this->addChild(m_pDebugSprite);
	m_bLoaded = true;
}

void SGUIStatic::unload() {
	if (m_bLoaded == false)
		return;
	removeAllChildren(); // autorelease 되기땜
	m_pDebugSprite = nullptr;
	CC_SAFE_RELEASE_NULL(m_pDebugTexture);
	m_bLoaded = false;
}

void SGUIStatic::setDebugVisible(bool visible) {
	m_bVisible = visible;

	if (m_pDebugSprite)
		m_pDebugSprite->setVisible(m_bVisible);
}



