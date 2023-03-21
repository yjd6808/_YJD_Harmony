/*
 * 작성자: 윤정도
 * 생성일: 2/23/2023 12:10:46 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGUICheckBox.h"
#include "GameCoreHeader.h"

USING_NS_CC;
USING_NS_JC;

SGUICheckBox::SGUICheckBox(SGUIGroup* parent, SGUICheckBoxInfo* checkBoxInfo)
	: SGUIElement(parent, checkBoxInfo)
	, m_pInfo(checkBoxInfo)
	, m_pTexture{}
	, m_pSprite{}
	, m_bChecked(false)
{}

SGUICheckBox::~SGUICheckBox() {
	for (int i = 0; i < TextureCount; ++i) {
		CC_SAFE_RELEASE(m_pTexture[i]);
	}
}

void SGUICheckBox::setCheck(bool checked) {

	if (m_eState == eDisabled)
		return;

	m_pSprite[IndexCross]->setVisible(checked);
	m_bChecked = checked;

	if (m_fnCheckStateChangedCallback)
		m_fnCheckStateChangedCallback(this, m_bChecked);
}

void SGUICheckBox::setEnabled(bool enabled) {

	if (enabled) {

		updateState();

		m_pSprite[IndexBackground]->setVisible(true);
		m_pSprite[IndexBackgroundDisabled]->setVisible(false);

		if (m_bChecked) {
			m_pSprite[IndexCross]->setVisible(true);
			m_pSprite[IndexCrossDisabled]->setVisible(false);
		}

		return;
	}


	m_pSprite[IndexBackground]->setVisible(false);
	m_pSprite[IndexBackgroundDisabled]->setVisible(true);

	if (m_bChecked) {
		m_pSprite[IndexCross]->setVisible(false);
		m_pSprite[IndexCrossDisabled]->setVisible(true);
	}

	m_eState = eDisabled;
}

bool SGUICheckBox::isChecked() const {
	return m_bChecked;
}

SGUICheckBox* SGUICheckBox::create(SGUIGroup* parent, SGUICheckBoxInfo* btnInfo) {
	SGUICheckBox* pCheckBox = dbg_new SGUICheckBox(parent, btnInfo);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}

bool SGUICheckBox::init() {

	const SGImagePack* pBackgroundPack = CorePackManager_v->getPackUnsafe(m_pInfo->BackgroundSga);
	const SGImagePack* pCrossPack = CorePackManager_v->getPackUnsafe(m_pInfo->CrossSga);

	// 백그라운드 팩은 없을 수도 있다. 크로스팩은 필수
	if (pCrossPack == nullptr) {
		return false;
	}

	SgaSpriteAbstractPtr spBackgroundSprite;
	SgaSpriteAbstractPtr spBackgroundDisabledSprite;

	if (pBackgroundPack != nullptr) {
		spBackgroundSprite = pBackgroundPack->getSpriteUnsafe(m_pInfo->BackgroundImg, m_pInfo->Sprites[IndexCross]);
		spBackgroundDisabledSprite = pBackgroundPack->getSpriteUnsafe(m_pInfo->BackgroundImg, m_pInfo->Sprites[IndexCrossDisabled]);
	}
	 
	SgaSpriteAbstractPtr spCrossSprite = pCrossPack->getSpriteUnsafe(m_pInfo->CrossImg, m_pInfo->Sprites[IndexCross]);
	SgaSpriteAbstractPtr spCrossDisabledSprite = pCrossPack->getSpriteUnsafe(m_pInfo->CrossImg, m_pInfo->Sprites[IndexCrossDisabled]);

	const float fWidth = SgaSpriteHelper::GetMaxWidthF(spBackgroundSprite, spBackgroundDisabledSprite, spCrossSprite, spCrossDisabledSprite);
	const float fHeight = SgaSpriteHelper::GetMaxHeightF(spBackgroundSprite, spBackgroundDisabledSprite, spCrossSprite, spCrossDisabledSprite);
	setContentSize(fWidth, fHeight);
	return true;
}

void SGUICheckBox::load() {
	if (m_bLoaded)
		return;

	SGImagePack* pBackgroundPack = CorePackManager_v->getPackUnsafe(m_pInfo->BackgroundSga);
	SGImagePack* pCrossPack = CorePackManager_v->getPack(m_pInfo->CrossSga);
	SGImagePack* pPack[TextureCount] = { pBackgroundPack, pBackgroundPack, pCrossPack, pCrossPack };
	const int Sga[TextureCount] { m_pInfo->BackgroundSga, m_pInfo->BackgroundSga, m_pInfo->CrossSga, m_pInfo->CrossSga };
	const int Img[TextureCount] { m_pInfo->BackgroundImg, m_pInfo->BackgroundImg, m_pInfo->CrossImg, m_pInfo->CrossImg };

	for (int i = 0; i < TextureCount; ++i) {
		const int iSprite = m_pInfo->Sprites[i];

		SGFrameTexture* pTexture = pPack[i] == nullptr ?
			CoreGlobal_v->getDefaultFrameTexture() :
			pPack[i]->createFrameTexture(Img[i], iSprite);
		pTexture->retain();

		Sprite* pSprite = Sprite::create();
		pSprite->initWithTexture(pTexture->getTexture());
		pSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		pSprite->setPosition(getContentSize() / 2);

		m_pTexture[i] = pTexture;
		m_pSprite[i] = pSprite;

		CoreUIManager_v->registerLoadedUITexture({ Sga[i], Img[i], iSprite});
		this->addChild(pSprite);
	}

	setEnabled(true);
	m_bLoaded = true;
}

void SGUICheckBox::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < TextureCount; ++i) {
		m_pSprite[i] = nullptr;
		CC_SAFE_RELEASE_NULL(m_pTexture[i]);
	}

	m_bLoaded = false;
}



bool SGUICheckBox::onMouseUp(SGEventMouse* mouseEvent) {
	if (m_eState != ePressed)
		return true;

	const Vec2 mousePos = mouseEvent->getCursorPos();
	const bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

	if (!bContainedMouse) {
		return true;
	}

	setCheck(!m_bChecked);

	if (m_fnMouseClickCallback)
		m_fnMouseClickCallback(mouseEvent);

	return false;
}

void SGUICheckBox::setCallbackCheckStateChanged(const SGActionFn<SGUICheckBox*, bool>& fnCheckStateChangedCallback) {
	m_fnCheckStateChangedCallback = fnCheckStateChangedCallback;
}