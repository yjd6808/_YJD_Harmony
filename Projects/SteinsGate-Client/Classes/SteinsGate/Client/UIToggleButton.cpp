/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 1:18:28 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIToggleButton.h"

#include <SteinsGate/Client/UIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

UIToggleButton::UIToggleButton(UIMasterGroup* master, UIGroup* parent)
	: UIElement(master, parent)
	, m_eToggleState(ToggleState::eNormal)
	, m_pInfo(nullptr)
	, m_pTexture{}
	, m_pSprite{}
{}

UIToggleButton::UIToggleButton(UIMasterGroup* master, UIGroup* parent, UIToggleButtonInfo* btnInfo, bool infoOwner)
	: UIElement(master, parent, btnInfo, infoOwner)
	, m_eToggleState(ToggleState::eNormal)
	, m_pInfo(btnInfo)
	, m_pTexture{}
	, m_pSprite{}
{}

UIToggleButton::~UIToggleButton() {
	CC_SAFE_RELEASE(m_pTexture[StateOne][eNormal]);
	CC_SAFE_RELEASE(m_pTexture[StateOne][eOver]);
	CC_SAFE_RELEASE(m_pTexture[StateOne][ePressed]);
	CC_SAFE_RELEASE(m_pTexture[StateOne][eDisabled]);

	CC_SAFE_RELEASE(m_pTexture[StateTwo][eNormal]);
	CC_SAFE_RELEASE(m_pTexture[StateTwo][eOver]);
	CC_SAFE_RELEASE(m_pTexture[StateTwo][ePressed]);
	CC_SAFE_RELEASE(m_pTexture[StateTwo][eDisabled]);
}

void UIToggleButton::setToggleState(ToggleState state) {
	m_eToggleState = state;
	setVisibleState(m_eState);
}

UIToggleButton* UIToggleButton::create(UIMasterGroup* master, UIGroup* parent) {
	UIToggleButton* pBtn = dbg_new UIToggleButton(master, parent);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

UIToggleButton* UIToggleButton::create(UIMasterGroup* master, UIGroup* parent, UIToggleButtonInfo* btnInfo, bool infoOwner) {
	UIToggleButton* pBtn = dbg_new UIToggleButton(master, parent, btnInfo, infoOwner);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

void UIToggleButton::setVisibleState(State state) {

	// 로딩안됬으면 모두 nullptr이므로 널포익 발생함
	if (!m_bLoaded)
		return;

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < eMax; ++j) 
			m_pSprite[i][j]->setVisible(false);

	m_pSprite[(int)m_eToggleState][state]->setVisible(true);
}

void UIToggleButton::setEnabled(bool enabled) {

	if (enabled) {
		// 활성화 요청을 했지만 이미 활성화 상태인 경우는 무시
		if (m_eState != eDisabled)
			return;

		// 이전 상태가 비활성화 상태 인경우 현재 상태가 Pressed인지, Moved인지 체크해서 업데이트
		updateState();
		return;
	}

	m_eState = eDisabled;
	setVisibleState(eDisabled);
}

void UIToggleButton::setUISize(const SGSize& contentSize) {
	if (!m_bResizable)
		return;

	m_UISize = contentSize;

	if (!m_bLoaded)
		return;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < eMax; ++j) {
			FrameTexture* pTexture = m_pTexture[i][j];
			Sprite* pSprite = m_pSprite[i][j];

			if (pTexture == nullptr || pSprite == nullptr) {
				continue;
			}

			pSprite->setScaleX(m_UISize.width / pTexture->getWidthF());
			pSprite->setScaleY(m_UISize.height / pTexture->getHeightF());
		}
	}
}

void UIToggleButton::setInfo(UIElementInfo* info, bool infoOwner) {
	if (info->Type != UIElementType::ToggleButton) {
		logWarnInvalidInfo(info->Type);
		return;
	}

	if (m_bInfoOwner) {
		JCORE_DELETE_SAFE(m_pInfo);
	}

	m_pBaseInfo = info;
	m_pInfo = static_cast<UIToggleButtonInfo*>(info);
	m_bInfoOwner = infoOwner;
}

void UIToggleButton::setInfoToggleButton(UIToggleButtonInfo* info, bool infoOwner) {
	setInfo(info, infoOwner);
}

void UIToggleButton::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleState(eNormal);
}

bool UIToggleButton::init() {
	if (!UIElement::init()) {
		return false;
	}

	const ImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
	setInitialUISize(DefaultSize30);
	if (pPack == nullptr) {
		_LogWarn_("토글버튼 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteAbstractPtr spSprite = pPack->getSpriteUnsafe(m_pInfo->Img, m_pInfo->Sprites[StateOne][eNormal]);

	if (spSprite == nullptr) {
		_LogWarn_("토글버튼 노말 스프라이트를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteRect spriteRect = spSprite->GetRect();
	setInitialUISize({ spriteRect.GetWidthF(), spriteRect.GetHeightF() });
	return m_bInitialized = true;
}

void UIToggleButton::load() {
	if (m_bLoaded)
		return;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < eMax; ++j) {
			const int iSprite = m_pInfo->Sprites[i][j];
			FrameTexture* pTexture = CoreUIManager_v->createUITextureRetained(m_pInfo->Sga, m_pInfo->Img, iSprite, m_pInfo->LinearDodge);

			Sprite* pSprite = Sprite::create();
			pSprite->initWithTexture(pTexture->getTexture());
			pSprite->setAnchorPoint(Vec2::ZERO);
			pSprite->setScaleX(m_UISize.width / pTexture->getWidthF());
			pSprite->setScaleY(m_UISize.height / pTexture->getHeightF());

			m_pTexture[i][j] = pTexture;
			m_pSprite[i][j] = pSprite;

			this->addChild(pSprite);
		}
	}


	m_bLoaded = true;
	setVisibleState(eNormal);
}

void UIToggleButton::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < eMax; ++j) {
			m_pSprite[i][j] = nullptr;
			CC_SAFE_RELEASE_NULL(m_pTexture[i][j]);
		}
	}

	m_bLoaded = false;
}

void UIToggleButton::onMouseEnterInternalDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eOver);
}
void UIToggleButton::onMouseLeaveInternalDetail(SGEventMouse* mouseEvent) {
	setVisibleState(State::eNormal);
}

bool UIToggleButton::onMouseMoveInternalDetail(SGEventMouse* mouseEvent) {
	return true;
}
bool UIToggleButton::onMouseDownInternalDetail(SGEventMouse* mouseEvent) {
	setVisibleState(ePressed);
	return false;
}

void UIToggleButton::onMouseUpInternalDetail(SGEventMouse* mouseEvent) {
	setVisibleState(State::eNormal);
}

bool UIToggleButton::onMouseUpContainedInternalDetail(SGEventMouse* mouseEvent) {
	m_eToggleState = m_eToggleState == ToggleState::eNormal ? ToggleState::eToggled : ToggleState::eNormal;
	m_pMasterGroup->onToggleStateChanged(this, m_eToggleState);
	setVisibleState(State::eNormal);
	return false;
}

