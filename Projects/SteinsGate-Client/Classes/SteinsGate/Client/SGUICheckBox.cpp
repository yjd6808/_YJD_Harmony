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
	: SGUIElement(parent)
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

		if (m_eState != eDisabled)
			return;

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

bool SGUICheckBox::isChecked() {
	return m_bChecked;
}

SGUICheckBox* SGUICheckBox::create(SGUIGroup* parent, SGUICheckBoxInfo* btnInfo) {
	SGUICheckBox* pCheckBox = dbg_new SGUICheckBox(parent, btnInfo);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}

bool SGUICheckBox::init() {

	if (m_pInfo->BackgroundDisabled == InvalidValue_v) {
		m_pBackgroundDisabledSprite = Sprite::create();
		this->addChild(m_pBackgroundDisabledSprite);
	}

	if (m_pInfo->Background == InvalidValue_v) {
		m_pBackgroundSprite = Sprite::create();
		this->addChild(m_pBackgroundSprite);
	}

	if (m_pInfo->CrossDisabled == InvalidValue_v) {
		m_pCrossDisabledSprite = Sprite::create();
		this->addChild(m_pCrossDisabledSprite);
	}

	return true;
}

void SGUICheckBox::load() {
	if (m_bLoaded)
		return;

	SGImagePack* pPack = CorePackManager_v->getPack(m_pInfo->Sga);

	if (m_pInfo->Background != InvalidValue_v) {
		
		m_pBackgroundTexture = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Background);
		m_pBackgroundTexture->retain();

		DebugAssertMsg(!m_pBackgroundTexture->isLink(), "체크박스의 백그라운드 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
		CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Background });
		m_pBackgroundSprite = Sprite::create();
		m_pBackgroundSprite->initWithTexture(m_pBackgroundTexture->getTexture());
		m_pBackgroundSprite->setAnchorPoint(Vec2::ZERO);
		this->addChild(m_pBackgroundSprite);
	}

	if (m_pInfo->BackgroundDisabled != InvalidValue_v) {

		m_pBackgroundDisabledTexture = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->BackgroundDisabled);
		m_pBackgroundDisabledTexture->retain();

		DebugAssertMsg(!m_pBackgroundDisabledTexture->isLink(), "체크박스의 백그라운드 디세이블드 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
		CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->BackgroundDisabled });
		m_pBackgroundDisabledSprite = Sprite::create();
		m_pBackgroundDisabledSprite->initWithTexture(m_pBackgroundDisabledTexture->getTexture());
		m_pBackgroundDisabledSprite->setAnchorPoint(Vec2::ZERO);
		this->addChild(m_pBackgroundDisabledSprite);
	}

	m_pCrossTexture = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Cross);
	m_pCrossTexture->retain();

	DebugAssertMsg(!m_pCrossTexture->isLink(), "체크박스의 크로스 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
	CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Cross });
	m_pCrossSprite = Sprite::create();
	m_pCrossSprite->initWithTexture(m_pCrossTexture->getTexture());

	if (m_pInfo->Background == InvalidValue_v) {
		m_pCrossSprite->setAnchorPoint(Vec2::ZERO);
		this->addChild(m_pCrossSprite);
		setContentSize(m_pCrossSprite->getContentSize());
	} else {
		DebugAssert(m_pBackgroundSprite, "백그라운드 스프라이트가 없습니다.");
		m_pCrossSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_pCrossSprite->setPosition(m_pBackgroundSprite->getContentSize() / 2);
		m_pBackgroundSprite->addChild(m_pCrossSprite);
		setContentSize(m_pBackgroundSprite->getContentSize());
	}

	if (m_pInfo->CrossDisabled != InvalidValue_v) {

		m_pCrossDisabledTexture = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->CrossDisabled);
		m_pCrossDisabledTexture->retain();

		DebugAssertMsg(!m_pCrossDisabledTexture->isLink(), "체크박스의 크로스 디세이블드 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
		CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->CrossDisabled });
		m_pCrossDisabledSprite = Sprite::create();
		m_pCrossDisabledSprite->initWithTexture(m_pCrossDisabledTexture->getTexture());
		m_pCrossDisabledSprite->setAnchorPoint(Vec2::ZERO);

		if (m_pInfo->Background == InvalidValue_v) {
			m_pCrossDisabledSprite->setAnchorPoint(Vec2::ZERO);
			this->addChild(m_pCrossDisabledSprite);
		} else {
			m_pCrossDisabledSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			m_pCrossDisabledSprite->setPosition(m_pBackgroundSprite->getContentSize() / 2);
			m_pBackgroundSprite->addChild(m_pCrossDisabledSprite);

		}
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

	Vec2 mousePos = mouseEvent->getCursorPos();
	bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

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