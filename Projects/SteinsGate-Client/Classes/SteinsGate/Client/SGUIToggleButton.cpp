/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 1:18:28 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIToggleButton.h"

USING_NS_CC;
USING_NS_JC;

SGUIToggleButton::SGUIToggleButton(SGUIGroup* parent, SGUIToggleButtonInfo* btnInfo)
	: SGUIElement(parent, btnInfo)
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

void SGUIToggleButton::setVisibleState(State state) {

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < eMax; ++j) {
			m_pSprite[i][j]->setVisible(false);
		}
	}

	m_pSprite[m_iToggleState][state]->setVisible(true);
	m_eState = state;
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

SGUIToggleButton* SGUIToggleButton::create(SGUIGroup* parent, SGUIToggleButtonInfo* btnInfo) {
	SGUIToggleButton* pBtn = dbg_new SGUIToggleButton(parent, btnInfo);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

void SGUIToggleButton::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleState(eNormal);
}

bool SGUIToggleButton::init() {
	SGImagePackManager* pPackManager = SGImagePackManager::get();
	const SGImagePack* pPack = pPackManager->getPackUnsafe(m_pInfo->Sga);

	if (pPack == nullptr)
		return false;

	const SgaSpriteAbstractPtr spSprite = pPack->getSpriteUnsafe(m_pInfo->Img, m_pInfo->Sprites[StateOne][eNormal]);

	if (spSprite == nullptr)
	{
		DebugAssertMsg(false, "토글 버튼 스프라이트없음");
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


bool SGUIToggleButton::onMouseMove(SGEventMouse* mouseEvent) {
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

bool SGUIToggleButton::onMouseDown(SGEventMouse* mouseEvent) {
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

bool SGUIToggleButton::onMouseUp(SGEventMouse* mouseEvent) {
	if (m_eState != ePressed)
		return true;

	const Vec2 mousePos = mouseEvent->getCursorPos();
	const bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

	if (!bContainedMouse) {
		setVisibleState(eNormal);
		return true;
	}

	m_iToggleState = m_iToggleState == 0 ? 1 : 0;
	setVisibleState(eNormal);

	if (m_fnMouseClickCallback)
		m_fnMouseClickCallback(mouseEvent);

	return false;
}


