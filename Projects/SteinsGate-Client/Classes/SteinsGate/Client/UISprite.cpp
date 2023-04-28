/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:22 PM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UISprite.h"

#include <SteinsGate/Client/UIMasterGroup.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

UISprite::UISprite(UIMasterGroup* master, UIGroup* parent, UISpriteInfo* staticInfo)
	: UIElement(master, parent, staticInfo)
	, m_pInfo(staticInfo)
	, m_pTexture{}
	, m_pSprite{}
{}

UISprite::~UISprite() {
	CC_SAFE_RELEASE(m_pTexture);
}

UISprite* UISprite::create(UIMasterGroup* master, UIGroup* parent, UISpriteInfo* spriteInfo) {
	UISprite* pSprite = dbg_new UISprite(master, parent, spriteInfo);
	pSprite->init();
	pSprite->autorelease();
	return pSprite;
}

bool UISprite::init() {
	_contentSize = m_pInfo->Size;
	return true;
}

void UISprite::load() {
	if (m_bLoaded)
		return;

	ImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);

	if (pPack == nullptr)
		return;

	m_pTexture = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Sprite, m_pInfo->LinearDodge);
	m_pTexture->retain();

	if (m_pTexture->isLink()) {
		CC_SAFE_RELEASE_NULL(m_pTexture);
		return;
	}

	CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Sprite });

	const Size spriteSize = m_pTexture->getSize();
	const float fScaleX = m_pInfo->Size.width / spriteSize.width;
	const float fScaleY = m_pInfo->Size.height / spriteSize.height;

	m_pSprite = m_pInfo->Scale9 ? Scale9Sprite::create() : Sprite::create();
	m_pSprite->initWithTexture(m_pTexture->getTexture());
	m_pSprite->setAnchorPoint(Vec2::ZERO);
	m_pSprite->setScale(fScaleX, fScaleY);
	this->addChild(m_pSprite);
	m_bLoaded = true;
}

void UISprite::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜
	m_pSprite = nullptr;
	CC_SAFE_RELEASE_NULL(m_pTexture);
	m_bLoaded = false;
}


void UISprite::setContentSize(const SGSize& contentSize) {
	if (!m_bResizable)
		return;

	_contentSize = contentSize;

	if (!m_bLoaded) 
		return;

	if (m_pTexture == nullptr || m_pSprite == nullptr)
		return;

	m_pSprite->setScaleX(_contentSize.width / m_pTexture->getWidthF());
	m_pSprite->setScaleY(_contentSize.height / m_pTexture->getHeightF());
}
