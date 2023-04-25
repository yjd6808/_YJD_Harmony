/*
 * 작성자: 윤정도
 * 생성일: 2/23/2023 12:10:46 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "UICheckBox.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/UIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

UICheckBox::UICheckBox(UIMasterGroup* master, UIGroup* parent, UICheckBoxInfo* checkBoxInfo)
	: UIElement(master, parent, checkBoxInfo)
	, m_pInfo(checkBoxInfo)
	, m_pTexture{}
	, m_pSprite{}
	, m_bChecked(false)
{}

UICheckBox::~UICheckBox() {
	for (int i = 0; i < TextureCount; ++i) {
		CC_SAFE_RELEASE(m_pTexture[i]);
	}
}

UICheckBox* UICheckBox::create(UIMasterGroup* master, UIGroup* parent, UICheckBoxInfo* btnInfo) {
	UICheckBox* pCheckBox = dbg_new UICheckBox(master, parent, btnInfo);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}


void UICheckBox::setCheck(bool checked) {

	if (m_eState == eDisabled)
		return;

	m_pSprite[IndexCross]->setVisible(checked);
	m_bChecked = checked;

	if (m_bChecked != checked)
		m_pMasterGroup->onCheckedStateChanged(this);
}

void UICheckBox::setEnabled(bool enabled) {

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

bool UICheckBox::isChecked() const {
	return m_bChecked;
}


bool UICheckBox::init() {

	const ImagePack* pBackgroundPack = CorePackManager_v->getPackUnsafe(m_pInfo->BackgroundSga);
	const ImagePack* pCrossPack = CorePackManager_v->getPackUnsafe(m_pInfo->CrossSga);
	setContentSize(DefaultSize30);

	// 백그라운드 팩은 없을 수도 있다. 크로스팩은 필수
	if (pCrossPack == nullptr) {
		_LogWarn_("체크박스 크로스 Sga패키지를 찾지 못했습니다.");
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

void UICheckBox::load() {
	if (m_bLoaded)
		return;

	ImagePack* pBackgroundPack = CorePackManager_v->getPackUnsafe(m_pInfo->BackgroundSga);
	ImagePack* pCrossPack = CorePackManager_v->getPack(m_pInfo->CrossSga);
	ImagePack* pPack[TextureCount] = { pBackgroundPack, pBackgroundPack, pCrossPack, pCrossPack };
	const int Sga[TextureCount] { m_pInfo->BackgroundSga, m_pInfo->BackgroundSga, m_pInfo->CrossSga, m_pInfo->CrossSga };
	const int Img[TextureCount] { m_pInfo->BackgroundImg, m_pInfo->BackgroundImg, m_pInfo->CrossImg, m_pInfo->CrossImg };

	for (int i = 0; i < TextureCount; ++i) {
		const int iSprite = m_pInfo->Sprites[i];

		FrameTexture* pTexture = pPack[i] == nullptr ?
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

void UICheckBox::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < TextureCount; ++i) {
		m_pSprite[i] = nullptr;
		CC_SAFE_RELEASE_NULL(m_pTexture[i]);
	}

	m_bLoaded = false;
}


bool UICheckBox::onMouseUpContainedDetail(SGEventMouse* mouseEvent) {
	setCheck(!m_bChecked);
	return false;
}