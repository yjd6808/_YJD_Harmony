/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:55:50 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGUIButton.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGUIManager.h>

USING_NS_CC;
USING_NS_JC;

SGUIButton::SGUIButton(SGUIGroup* parent, SGUIButtonInfo* btnInfo)
	: SGUIElement(parent, btnInfo)
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

SGUIButton* SGUIButton::create(SGUIGroup* parent, SGUIButtonInfo* btnInfo) {
	SGUIButton* pBtn = dbg_new SGUIButton(parent,btnInfo);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

void SGUIButton::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleState(eNormal);
}

bool SGUIButton::init() {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	const SGImagePack* pPack = pPackManager->getPackUnsafe(m_pInfo->Sga);

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


bool SGUIButton::onMouseMove(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled ||
		m_eState == ePressed)
		return true;

	const Vec2 mousePos = mouseEvent->getCursorPos();
	const SGRect worldRect = getWorldBoundingBox();
	const bool bContainedMouse = worldRect.containsPoint(mousePos);

	if (!bContainedMouse) {
		setVisibleState(eNormal);
		return true;
	}

	setVisibleState(eOver);
	return false;
}

bool SGUIButton::onMouseDown(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled ||
		m_eState == ePressed)
		return true;

	const Vec2 mousePos = mouseEvent->getCursorPos();
	const bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

	if (!bContainedMouse) {
		return true;
	}

	setVisibleState(ePressed);
	return false;
}

bool SGUIButton::onMouseUp(SGEventMouse* mouseEvent) {
	if (m_eState != ePressed)
		return true;

	const Vec2 mousePos = mouseEvent->getCursorPos();
	const bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

	setVisibleState(eNormal);

	if (!bContainedMouse) {
		return true;
	}

	if (m_fnMouseClickCallback)
		m_fnMouseClickCallback(mouseEvent);
	
	return false;
}

bool SGUIButton::onMouseScroll(SGEventMouse* mouseEvent) {
	return true;
}
