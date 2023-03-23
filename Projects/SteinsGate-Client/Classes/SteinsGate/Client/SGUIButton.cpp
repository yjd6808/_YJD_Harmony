/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:55:50 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIButton.h"

#include <SteinsGate/Client/SGUIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

SGUIButton::SGUIButton(SGUIMasterGroup* master, SGUIGroup* parent, SGUIButtonInfo* btnInfo)
	: SGUIElement(master, parent, btnInfo)
	, m_pInfo(btnInfo)
	, m_pTexture{}
	, m_pSprite{}
{}

SGUIButton::~SGUIButton() {
	CC_SAFE_RELEASE(m_pTexture[eNormal]);
	CC_SAFE_RELEASE(m_pTexture[eOver]);
	CC_SAFE_RELEASE(m_pTexture[ePressed]);
	CC_SAFE_RELEASE(m_pTexture[eDisabled]);
}

SGUIButton* SGUIButton::create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIButtonInfo* btnInfo) {
	SGUIButton* pBtn = dbg_new SGUIButton(master, parent, btnInfo);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}


void SGUIButton::setVisibleState(State state) {
	for (int i = 0; i < eMax; ++i) {
		if (i == state) {
			m_pSprite[i]->setVisible(true);
			continue;
		}
		m_pSprite[i]->setVisible(false);
	}

	m_eState = state;
}

void SGUIButton::setEnabled(bool enabled) {

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


void SGUIButton::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleState(eNormal);
}

void SGUIButton::onMouseEnterDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eOver);
}

void SGUIButton::onMouseLeaveDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eNormal);
}

bool SGUIButton::onMouseMoveDetail(SGEventMouse* mouseEvent) {
	return false;
}
bool SGUIButton::onMouseDownDetail(SGEventMouse* mouseEvent) {
	setVisibleState(ePressed);
	return false;
}
void SGUIButton::onMouseUpDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eNormal);
}


bool SGUIButton::init() {
	const SGImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
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

void SGUIButton::load() { 
	if (m_bLoaded)
		return;

	SGImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);

	for (int i = 0; i < eMax; ++i) {
		const int iSprite = m_pInfo->Sprites[i];

		SGFrameTexture* pTexture = pPack->createFrameTexture(m_pInfo->Img, iSprite, m_pInfo->LinearDodge);
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

void SGUIButton::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < eMax; ++i) {
		m_pSprite[i] = nullptr;
		CC_SAFE_RELEASE_NULL(m_pTexture[eNormal]);
	}

	m_bLoaded = false;
}


