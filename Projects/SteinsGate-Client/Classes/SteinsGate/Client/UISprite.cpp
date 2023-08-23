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

UISprite::UISprite(UIMasterGroup* master, UIGroup* parent)
	: UIElement(master, parent)
	, m_pInfo(nullptr)
	, m_pTexture{}
	, m_pSprite{}
{}

UISprite::UISprite(UIMasterGroup* master, UIGroup* parent, UISpriteInfo* spriteInfo, bool infoOwner)
	: UIElement(master, parent, spriteInfo, infoOwner)
	, m_pInfo(spriteInfo)
	, m_pTexture{}
	, m_pSprite{}
{}

UISprite::~UISprite() {
	CC_SAFE_RELEASE(m_pTexture);
}

UISprite* UISprite::create(UIMasterGroup* master, UIGroup* parent) {
	UISprite* pSprite = dbg_new UISprite(master, parent);
	pSprite->init();
	pSprite->autorelease();
	return pSprite;
}

UISprite* UISprite::create(UIMasterGroup* master, UIGroup* parent, UISpriteInfo* spriteInfo, bool infoOwner) {
	UISprite* pSprite = dbg_new UISprite(master, parent, spriteInfo, infoOwner);
	pSprite->init();
	pSprite->autorelease();
	return pSprite;
}

bool UISprite::init() {
	if (!UIElement::init()) {
		return false;
	}

	SGSize initialSize = m_pInfo->Size;

	if (int(initialSize.width) == 0 || int(initialSize.width) == 0) {
		initialSize = Core::Contents.PackManager->getTextureSize(m_pInfo->Sga, m_pInfo->Img, m_pInfo->Sprite);
	}

	setInitialUISize(initialSize);
	return m_bInitialized = true;
}

void UISprite::load() {
	if (m_bLoaded)
		return;

	m_pTexture = Core::Contents.UIManager->createUITextureRetained(m_pInfo->Sga, m_pInfo->Img, m_pInfo->Sprite, m_pInfo->LinearDodge);

	if (m_pTexture->isLink()) {
		CC_SAFE_RELEASE_NULL(m_pTexture);
		return;
	}

	const Size spriteSize = m_pTexture->getSize();
	float fScaleX = 1.0f;
	float fScaleY = 1.0f;

	if (m_UISize.width > 0.0f) {
		fScaleX = m_UISize.width / spriteSize.width;
	}

	if (m_UISize.height > 0.0f) {
		fScaleY = m_UISize.height / spriteSize.height;
	}

	m_pSprite = m_pInfo->Scale9 ? Scale9Sprite::create() : Sprite::create();
	m_pSprite->initWithTexture(m_pTexture->getTexture());
	m_pSprite->setAnchorPoint(Vec2::ZERO);
	if (m_pInfo->Scale9) {
		m_pSprite->setContentSize({ spriteSize.width * fScaleX, spriteSize.height * fScaleY });
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



void UISprite::setInfo(UIElementInfo* info, bool infoOwner) {
	if (info->Type != UIElementType::Sprite) {
		logWarnInvalidInfo(info->Type);
		return;
	}

	if (m_bInfoOwner) {
		JCORE_DELETE_SAFE(m_pInfo);
	}

	m_pBaseInfo = info;
	m_pInfo = static_cast<UISpriteInfo*>(info);
	m_bInfoOwner = infoOwner;
}

void UISprite::setInfoSprite(UISpriteInfo* info, bool infoOwner) {
	return setInfo(info, infoOwner);
}

void UISprite::setOpacity(GLubyte opacity) {
	UIElement::setOpacity(opacity);

	if (m_pSprite)
		m_pSprite->setOpacity(opacity);
}
