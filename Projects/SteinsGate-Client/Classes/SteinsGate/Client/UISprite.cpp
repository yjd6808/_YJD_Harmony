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
	setInitialUISize(m_pInfo->Size);
	return m_bInitialized = true;
}

void UISprite::load() {
	if (m_bLoaded)
		return;

	m_pTexture = CoreUIManager_v->createUITextureRetained(m_pInfo->Sga, m_pInfo->Img, m_pInfo->Sprite, m_pInfo->LinearDodge);

	if (m_pTexture->isLink()) {
		CC_SAFE_RELEASE_NULL(m_pTexture);
		return;
	}

	const Size spriteSize = m_pTexture->getSize();
	const float fScaleX = m_UISize.width / spriteSize.width;
	const float fScaleY = m_UISize.height / spriteSize.height;

	m_pSprite = m_pInfo->Scale9 ? Scale9Sprite::create() : Sprite::create();
	m_pSprite->initWithTexture(m_pTexture->getTexture());
	m_pSprite->setAnchorPoint(Vec2::ZERO);
	if (m_pInfo->Scale9) {
		m_pSprite->setContentSize({m_UISize.width * fScaleX, m_UISize.height * fScaleY });
	} else {
		m_pSprite->setScale(fScaleX, fScaleY);
	}
	
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

void UISprite::setCapInsets(const SGRect& insets) {
	if (!m_pInfo->Scale9) {
		_LogWarn_("스케일9 스프가 아닌데 setCapInsets 시도");
		return;
	}

	if (!m_bLoaded) {
		_LogWarn_("로딩 안됬는데 setCapInsets 시도");
		return;
	}

	dynamic_cast<Scale9Sprite*>(m_pSprite)->setCapInsets(insets);
}


void UISprite::setUISize(const SGSize& contentSize) {
	if (!m_bResizable)
		return;

	m_UISize = contentSize;

	if (!m_bLoaded) 
		return;

	if (m_pTexture == nullptr || m_pSprite == nullptr)
		return;

	const Size spriteSize = m_pTexture->getSize();
	const float fScaleX = m_UISize.width / spriteSize.width;
	const float fScaleY = m_UISize.height / spriteSize.height;

	if (m_pInfo->Scale9) {
		m_pSprite->setContentSize({ spriteSize.width * fScaleX, spriteSize.height * fScaleY });
	} else {
		m_pSprite->setScale(fScaleX, fScaleY);
	}
	
}
