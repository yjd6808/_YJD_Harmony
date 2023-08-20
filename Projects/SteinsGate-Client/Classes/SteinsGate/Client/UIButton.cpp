/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:55:50 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIButton.h"

#include <SteinsGate/Client/UIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

UIButton::UIButton(UIMasterGroup* master, UIGroup* parent)
	: UIElement(master, parent)
	, m_pInfo(nullptr)
	, m_pTexture{}
	, m_pSprite{}
{}

UIButton::UIButton(UIMasterGroup* master, UIGroup* parent, UIButtonInfo* btnInfo, bool infoOwner)
	: UIElement(master, parent, btnInfo, infoOwner)
	, m_pInfo(btnInfo)
	, m_pTexture{}
	, m_pSprite{}
{}

UIButton::~UIButton() {
	CC_SAFE_RELEASE(m_pTexture[eNormal]);
	CC_SAFE_RELEASE(m_pTexture[eOver]);
	CC_SAFE_RELEASE(m_pTexture[ePressed]);
	CC_SAFE_RELEASE(m_pTexture[eDisabled]);
}

UIButton* UIButton::create(UIMasterGroup* master, UIGroup* parent) {
	UIButton* pBtn = dbg_new UIButton(master, parent);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

UIButton* UIButton::create(UIMasterGroup* master, UIGroup* parent, UIButtonInfo* btnInfo, bool infoOwner) {
	UIButton* pBtn = dbg_new UIButton(master, parent, btnInfo, infoOwner);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}


void UIButton::setVisibleState(State state) {
	for (int i = 0; i < eMax; ++i) {
		Sprite* pSprite = m_pSprite[i];

		if (pSprite == nullptr)
			continue;

		if (i == state) {
			pSprite->setVisible(true);
			continue;
		}
		pSprite->setVisible(false);
	}

	m_eState = state;
}

void UIButton::setUISize(const SGSize& size) {
	if (!m_bResizable)
		return;

	m_UISize = size;

	if (!m_bLoaded) 
		return;

	const float fScaleX = getScaleX();
	const float fScaleY = getScaleY();

	for (int i = 0; i < eMax; ++i) {

		Sprite* pSprite = m_pSprite[i];

		if (pSprite == nullptr) {
			continue;
		}

		pSprite->setScaleX(fScaleX);
		pSprite->setScaleY(fScaleY);
	}
	
}

void UIButton::setInfo(UIElementInfo* info, bool infoOwner) {

	if (info->Type != UIElementType::Button) {
		logWarnInvalidInfo(info->Type);
		return;
	}

	if (m_bInfoOwner) {
		JCORE_DELETE_SAFE(m_pInfo);
	}

	m_pBaseInfo = info;
	m_pInfo = static_cast<UIButtonInfo*>(info);
	m_bInfoOwner = infoOwner;
}

void UIButton::setInfoButton(UIButtonInfo* info, bool infoOwner) {
	setInfo(info, infoOwner);
}


void UIButton::setEnabled(bool enabled) {

	if (enabled) {
		// 활성화 요청을 했지만 이미 활성화 상태인 경우는 무시
		if (m_eState != eDisabled)
			return;

		// 이전 상태가 비활성화 상태 인경우 현재 상태가 Pressed인지, Moved인지 체크해서 업데이트
		updateState();
		setVisibleState(eNormal);
		return;
	}

	m_eState = eDisabled;
	setVisibleState(eDisabled);
}

void UIButton::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleState(eNormal);
}

void UIButton::onMouseEnterInternalDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eOver);
}

void UIButton::onMouseLeaveInternalDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eNormal);
}

bool UIButton::onMouseMoveInternalDetail(SGEventMouse* mouseEvent) {
	return false;
}
bool UIButton::onMouseDownInternalDetail(SGEventMouse* mouseEvent) {
	setVisibleState(ePressed);
	return false;
}
void UIButton::onMouseUpInternalDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eNormal);
}


bool UIButton::init() {

	if (!UIElement::init()) {
		return false;
	}

	if (m_pInfo == nullptr) {
		logWarnMissingInfo();
		return false;
	}

	const ImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
	setInitialUISize(DefaultSize30);

	if (pPack == nullptr) {
		_LogWarn_("버튼 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteAbstractPtr spSprite = pPack->getSpriteUnsafe(m_pInfo->Img, m_pInfo->Sprites[eNormal]);

	if (spSprite == nullptr) {
		_LogWarn_("버튼 노말 스프라이트를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteRect spriteRect = spSprite->GetRect();
	setInitialUISize({ spriteRect.GetWidthF(), spriteRect.GetHeightF() });
	return m_bInitialized = true;
}

void UIButton::load() { 
	if (m_bLoaded)
		return;

	for (int i = 0; i < eMax; ++i) {
		const int iSprite = m_pInfo->Sprites[i];

		if (iSprite == InvalidValue_v) {
			_LogWarn_("설정되지 않은 스프라이트입니다.");
			continue;
		}

		FrameTexture* pTexture = CoreUIManager_v->createUITextureRetained(m_pInfo->Sga, m_pInfo->Img, iSprite, m_pInfo->LinearDodge);

		Sprite* pSprite = Sprite::create();
		pSprite->initWithTexture(pTexture->getTexture());
		pSprite->setScaleX(m_UISize.width / pTexture->getWidthF());
		pSprite->setScaleY(m_UISize.height / pTexture->getHeightF());
		pSprite->setAnchorPoint(Vec2::ZERO);

		m_pTexture[i] = pTexture;
		m_pSprite[i] = pSprite;

		this->addChild(pSprite);
	}

	setVisibleState(eNormal);

	m_bLoaded = true;
}

void UIButton::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < eMax; ++i) {
		m_pSprite[i] = nullptr;
		CC_SAFE_RELEASE_NULL(m_pTexture[i]);
	}

	m_bLoaded = false;
}


