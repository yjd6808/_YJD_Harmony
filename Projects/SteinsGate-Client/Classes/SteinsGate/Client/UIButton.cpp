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

UIButton::UIButton(UIMasterGroup* master, UIGroup* parent, UIButtonInfo* btnInfo)
	: UIElement(master, parent, btnInfo)
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

UIButton* UIButton::create(UIMasterGroup* master, UIGroup* parent, UIButtonInfo* btnInfo) {
	UIButton* pBtn = dbg_new UIButton(master, parent, btnInfo);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}


void UIButton::setVisibleState(State state) {
	for (int i = 0; i < eMax; ++i) {
		if (i == state) {
			m_pSprite[i]->setVisible(true);
			continue;
		}
		m_pSprite[i]->setVisible(false);
	}

	m_eState = state;
}

void UIButton::setEnabled(bool enabled) {

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


void UIButton::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleState(eNormal);
}

void UIButton::onMouseEnterDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eOver);
}

void UIButton::onMouseLeaveDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eNormal);
}

bool UIButton::onMouseMoveDetail(SGEventMouse* mouseEvent) {
	return false;
}
bool UIButton::onMouseDownDetail(SGEventMouse* mouseEvent) {
	setVisibleState(ePressed);
	return false;
}
void UIButton::onMouseUpDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eNormal);
}


bool UIButton::init() {
	const ImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
	setContentSize(DefaultSize30);

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
	setContentSize({ spriteRect.GetWidthF(), spriteRect.GetHeightF() });
	return true;
}

void UIButton::load() { 
	if (m_bLoaded)
		return;

	ImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);

	for (int i = 0; i < eMax; ++i) {
		const int iSprite = m_pInfo->Sprites[i];

		FrameTexture* pTexture = pPack->createFrameTexture(m_pInfo->Img, iSprite, m_pInfo->LinearDodge);
		pTexture->retain();

		Sprite* pSprite = Sprite::create();
		pSprite = Sprite::create();
		pSprite->initWithTexture(pTexture->getTexture());
		pSprite->setAnchorPoint(Vec2::ZERO);

		m_pTexture[i] = pTexture;
		m_pSprite[i] = pSprite;

		CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, iSprite });
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
		CC_SAFE_RELEASE_NULL(m_pTexture[eNormal]);
	}

	m_bLoaded = false;
}

