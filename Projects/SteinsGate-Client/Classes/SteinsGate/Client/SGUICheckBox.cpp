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
	, m_pBackgroundTexture{}
	, m_pBackgroundDisabledTexture{}
	, m_pCrossTexture{}
	, m_pCrossDisabledTexture{}
	, m_pBackgroundSprite{}
	, m_pBackgroundDisabledSprite{}
	, m_pCrossSprite{}
	, m_pCrossDisabledSprite{}
	, m_bChecked(false)
{}

SGUICheckBox::~SGUICheckBox() {
	CC_SAFE_RELEASE(m_pBackgroundTexture);
	CC_SAFE_RELEASE(m_pBackgroundDisabledTexture);
	CC_SAFE_RELEASE(m_pCrossTexture);
	CC_SAFE_RELEASE(m_pCrossDisabledTexture);
}

void SGUICheckBox::setCheck(bool checked) {

	if (m_eState == eDisabled)
		return;

	m_pCrossSprite->setVisible(checked);
	m_bChecked = checked;

	if (m_fnCheckStateChangedCallback)
		m_fnCheckStateChangedCallback(this, m_bChecked);
}

void SGUICheckBox::setEnabled(bool enabled) {

	if (enabled) {

		updateState();

		m_pBackgroundSprite->setVisible(true);
		m_pBackgroundDisabledSprite->setVisible(false);

		if (m_bChecked) {
			m_pCrossSprite->setVisible(true);
			m_pCrossDisabledSprite->setVisible(false);
		}

		return;
	}


	m_pBackgroundSprite->setVisible(false);
	m_pBackgroundDisabledSprite->setVisible(true);

	if (m_bChecked) {
		m_pCrossSprite->setVisible(false);
		m_pCrossDisabledSprite->setVisible(true);
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

	if (m_pInfo->Sprites[IndexBackground] == InvalidValue_v) {
		m_pBackgroundSprite = Sprite::create();
		this->addChild(m_pBackgroundSprite);
	}

	if (m_pInfo->Sprites[IndexBackgroundDisabled] == InvalidValue_v) {
		m_pBackgroundDisabledSprite = Sprite::create();
		this->addChild(m_pBackgroundDisabledSprite);
	}

	const SGImagePack* pBackgroundPack = CorePackManager_v->getPackUnsafe(m_pInfo->BackgroundSga);
	const SGImagePack* pCrossPack = CorePackManager_v->getPack(m_pInfo->CrossSga);

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

	const int iBackground = m_pInfo->Sprites[IndexBackground];
	const int iBackgroundDisabled = m_pInfo->Sprites[IndexBackgroundDisabled];
	const int iCross = m_pInfo->Sprites[IndexCross];
	const int iCrossDisabled = m_pInfo->Sprites[IndexCrossDisabled];

	if (iBackground != InvalidValue_v) {

		m_pBackgroundTexture = pBackgroundPack->createFrameTexture(m_pInfo->BackgroundImg, iBackground);
		m_pBackgroundTexture->retain();

		DebugAssertMsg(!m_pBackgroundTexture->isLink(), "체크박스의 백그라운드 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
		CoreUIManager_v->registerLoadedUITexture({ m_pInfo->BackgroundSga, m_pInfo->BackgroundImg, iBackground });
		m_pBackgroundSprite = Sprite::create();
		m_pBackgroundSprite->initWithTexture(m_pBackgroundTexture->getTexture());
		m_pBackgroundSprite->setAnchorPoint(Vec2::ZERO);
		this->addChild(m_pBackgroundSprite);
	}

	if (iBackgroundDisabled != InvalidValue_v) {

		m_pBackgroundDisabledTexture = pBackgroundPack->createFrameTexture(m_pInfo->BackgroundImg, iBackgroundDisabled);
		m_pBackgroundDisabledTexture->retain();

		DebugAssertMsg(!m_pBackgroundDisabledTexture->isLink(), "체크박스의 백그라운드 디세이블드 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
		CoreUIManager_v->registerLoadedUITexture({ m_pInfo->BackgroundSga, m_pInfo->BackgroundImg, iBackgroundDisabled });
		m_pBackgroundDisabledSprite = Sprite::create();
		m_pBackgroundDisabledSprite->initWithTexture(m_pBackgroundDisabledTexture->getTexture());
		m_pBackgroundDisabledSprite->setAnchorPoint(Vec2::ZERO);
		m_pBackgroundDisabledSprite->setVisible(false);
		this->addChild(m_pBackgroundDisabledSprite);
	}

	m_pCrossTexture = pCrossPack->createFrameTexture(m_pInfo->CrossImg, iCross);
	m_pCrossTexture->retain();

	DebugAssertMsg(!m_pCrossTexture->isLink(), "체크박스의 크로스 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
	CoreUIManager_v->registerLoadedUITexture({ m_pInfo->CrossSga, m_pInfo->CrossImg, iCross });
	m_pCrossSprite = Sprite::create();
	m_pCrossSprite->initWithTexture(m_pCrossTexture->getTexture());

	if (iBackground == InvalidValue_v) {
		m_pCrossSprite->setAnchorPoint(Vec2::ZERO);
		this->addChild(m_pCrossSprite);
		setContentSize(m_pCrossSprite->getContentSize());
	} else {
		DebugAssertMsg(m_pBackgroundSprite, "백그라운드 스프라이트가 없습니다.");
		m_pCrossSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pCrossSprite->setPosition(m_pBackgroundSprite->getContentSize() / 2);
		m_pBackgroundSprite->addChild(m_pCrossSprite);
		setContentSize(m_pBackgroundSprite->getContentSize());
	}

	if (iCrossDisabled != InvalidValue_v) {

		m_pCrossDisabledTexture = pCrossPack->createFrameTexture(m_pInfo->CrossImg, iCrossDisabled);
		m_pCrossDisabledTexture->retain();

		DebugAssertMsg(!m_pCrossDisabledTexture->isLink(), "체크박스의 크로스 디세이블드 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
		CoreUIManager_v->registerLoadedUITexture({ m_pInfo->CrossSga, m_pInfo->CrossImg, iCrossDisabled });
	} else {
		// 만약 비활성화 Cross 텍스쳐가 없는 경우, 활성화 텍스쳐를 그대로 쓰도록 하자.
		m_pCrossDisabledTexture = m_pCrossTexture;
		m_pCrossDisabledTexture->retain();
	}

	m_pCrossDisabledSprite = Sprite::create();
	m_pCrossDisabledSprite->initWithTexture(m_pCrossDisabledTexture->getTexture());
	m_pCrossDisabledSprite->setAnchorPoint(Vec2::ZERO);
	m_pCrossDisabledSprite->setVisible(false);


	if (iBackground == InvalidValue_v) {
		m_pCrossDisabledSprite->setAnchorPoint(Vec2::ZERO);
		this->addChild(m_pCrossDisabledSprite);
	} else {
		m_pCrossDisabledSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pCrossDisabledSprite->setPosition(m_pBackgroundSprite->getContentSize() / 2);
		m_pBackgroundSprite->addChild(m_pCrossDisabledSprite);
	}

	setEnabled(true);
	m_bLoaded = true;
}

void SGUICheckBox::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	m_pBackgroundSprite = nullptr;
	m_pBackgroundDisabledSprite = nullptr;
	m_pCrossSprite = nullptr;
	m_pCrossDisabledSprite = nullptr;

	CC_SAFE_RELEASE_NULL(m_pBackgroundTexture);
	CC_SAFE_RELEASE_NULL(m_pBackgroundDisabledTexture);
	CC_SAFE_RELEASE_NULL(m_pCrossTexture);
	CC_SAFE_RELEASE_NULL(m_pCrossDisabledTexture);

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


int SGUICheckBox::getCode() {
	return m_pInfo->Code;
}


void SGUICheckBox::setCallbackCheckStateChanged(const SGActionFn<SGUICheckBox*, bool>& fnCheckStateChangedCallback) {
	m_fnCheckStateChangedCallback = fnCheckStateChangedCallback;
}