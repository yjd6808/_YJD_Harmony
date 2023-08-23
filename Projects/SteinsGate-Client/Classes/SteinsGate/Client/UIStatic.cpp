/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:43 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIStatic.h"

#include <SteinsGate/Client/UIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

UIStatic::UIStatic(UIMasterGroup* master, UIGroup* parent)
	: UIElement(master, parent)
	, m_bVisible(false)
	, m_pInfo(nullptr)
	, m_pDebugTexture{}
	, m_pDebugSprite{}
{}

UIStatic::UIStatic(UIMasterGroup* master, UIGroup* parent, UIStaticInfo* staticInfo, bool infoOwner)
	: UIElement(master, parent, staticInfo, infoOwner)
	, m_bVisible(false)
	, m_pInfo(staticInfo)
	, m_pDebugTexture{}
	, m_pDebugSprite{}
{}

UIStatic::~UIStatic() {
	CC_SAFE_RELEASE(m_pDebugTexture);
}

UIStatic* UIStatic::create(UIMasterGroup* master, UIGroup* parent) {
	UIStatic* pStatic = dbg_new UIStatic(master, parent);
	pStatic->init();
	pStatic->autorelease();
	return pStatic;
}

UIStatic* UIStatic::create(UIMasterGroup* master, UIGroup* parent, UIStaticInfo* staticInfo, bool infoOwner) {
	UIStatic* pStatic = dbg_new UIStatic(master, parent, staticInfo, infoOwner);
	pStatic->init();
	pStatic->autorelease();
	return pStatic;
}

bool UIStatic::init() {
	if (!UIElement::init()) {
		return false;
	}

	if (m_pInfo == nullptr) {
		logWarnMissingInfo();
		return false;
	}

	setInitialUISize(m_pInfo->Size);
	return m_bInitialized = true;
}

void UIStatic::load() {
	if (m_bLoaded)
		return;

	m_pDebugTexture = Core::Contents.Global->getDefaultFrameTexture();
	m_pDebugTexture->retain();

	const Size spriteSize = m_pDebugTexture->getSize();
	const float fScaleX = m_UISize.width / spriteSize.width;
	const float fScaleY = m_UISize.height / spriteSize.height;

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

void UIStatic::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜
	m_pDebugSprite = nullptr;
	CC_SAFE_RELEASE_NULL(m_pDebugTexture);
	m_bLoaded = false;
}

void UIStatic::setDebugVisible(bool visible) {
	m_bVisible = visible;

	if (m_pDebugSprite)
		m_pDebugSprite->setVisible(m_bVisible);
}

void UIStatic::setUISize(const SGSize& contentSize) {
	if (!m_bResizable)
		return;

	UIElement::setContentSize(contentSize);

	if (!m_bLoaded)
		return;

	m_pDebugTexture = Core::Contents.Global->getDefaultFrameTexture();

	const Size spriteSize = m_pDebugTexture->getSize();
	const float fScaleX = m_UISize.width / spriteSize.width;
	const float fScaleY = m_UISize.height / spriteSize.height;

	m_pDebugSprite->setScale(fScaleX, fScaleY);

}

void UIStatic::setInfo(UIElementInfo* info, bool infoOwner) {
	if (info->Type != UIElementType::Static) {
		logWarnInvalidInfo(info->Type);
		return;
	}

	if (m_bInfoOwner) {
		JCORE_DELETE_SAFE(m_pInfo);
	}

	m_pBaseInfo = info;
	m_pInfo = static_cast<UIStaticInfo*>(info);
	m_bInfoOwner = infoOwner;
}

void UIStatic::setInfoStatic(UIStaticInfo* info, bool infoOwner) {
	setInfo(info, infoOwner);
}



