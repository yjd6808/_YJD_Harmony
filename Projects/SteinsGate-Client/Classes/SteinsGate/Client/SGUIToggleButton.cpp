/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 1:18:28 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIToggleButton.h"

#include <SteinsGate/Client/SGUIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

SGUIToggleButton::SGUIToggleButton(SGUIMasterGroup* master, SGUIGroup* parent, SGUIToggleButtonInfo* btnInfo)
	: SGUIElement(master, parent, btnInfo)
	, m_iToggleState(StateOne)
	, m_pInfo(btnInfo)
	, m_pTexture{}
	, m_pSprite{}
{}

SGUIToggleButton::~SGUIToggleButton() {
	CC_SAFE_RELEASE(m_pTexture[StateOne][eNormal]);
	CC_SAFE_RELEASE(m_pTexture[StateOne][eOver]);
	CC_SAFE_RELEASE(m_pTexture[StateOne][ePressed]);
	CC_SAFE_RELEASE(m_pTexture[StateOne][eDisabled]);

	CC_SAFE_RELEASE(m_pTexture[StateTwo][eNormal]);
	CC_SAFE_RELEASE(m_pTexture[StateTwo][eOver]);
	CC_SAFE_RELEASE(m_pTexture[StateTwo][ePressed]);
	CC_SAFE_RELEASE(m_pTexture[StateTwo][eDisabled]);
}

SGUIToggleButton* SGUIToggleButton::create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIToggleButtonInfo* btnInfo) {
	SGUIToggleButton* pBtn = dbg_new SGUIToggleButton(master, parent, btnInfo);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

void SGUIToggleButton::setVisibleState(State state) {

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < eMax; ++j) {
			m_pSprite[i][j]->setVisible(false);
		}
	}

	m_pSprite[m_iToggleState][state]->setVisible(true);
}

void SGUIToggleButton::setEnabled(bool enabled) {

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

void SGUIToggleButton::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleState(eNormal);
}

bool SGUIToggleButton::init() {
	const SGImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
	setContentSize(DefaultSize30);

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
	setContentSize({ spriteRect.GetWidthF(), spriteRect.GetHeightF() });
	return true;
}

void SGUIToggleButton::load() {
	if (m_bLoaded)
		return;

	SGImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < eMax; ++j) {
			const int iSprite = m_pInfo->Sprites[i][j];
			SGFrameTexture* pTexture = pPack == nullptr ? 
				CoreGlobal_v->getDefaultFrameTexture() :
				pPack->createFrameTexture(m_pInfo->Img, iSprite, m_pInfo->LinearDodge);

			pTexture->retain();

			Sprite* pSprite = Sprite::create();
			pSprite = Sprite::create();
			pSprite->initWithTexture(pTexture->getTexture());
			pSprite->setAnchorPoint(Vec2::ZERO);

			m_pTexture[i][j] = pTexture;
			m_pSprite[i][j] = pSprite;

			CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, iSprite });
			this->addChild(pSprite);
		}
	}

	
	setVisibleState(eNormal);
	m_bLoaded = true;
}

void SGUIToggleButton::unload() {
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

void SGUIToggleButton::onMouseEnterDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eOver);
}
void SGUIToggleButton::onMouseLeaveDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eNormal);
}

bool SGUIToggleButton::onMouseMoveDetail(SGEventMouse* mouseEvent) {
	return false;
}
bool SGUIToggleButton::onMouseDownDetail(SGEventMouse* mouseEvent) {
	setVisibleState(ePressed);
	return false;
}

void SGUIToggleButton::onMouseUpDetail(SGEventMouse* mouseEvent) {
	setVisibleState(eNormal);
}

bool SGUIToggleButton::onMouseUpContainedDetail(SGEventMouse* mouseEvent) {
	m_iToggleState = m_iToggleState == 0 ? 1 : 0;
	setVisibleState(eNormal);
	return false;
}

