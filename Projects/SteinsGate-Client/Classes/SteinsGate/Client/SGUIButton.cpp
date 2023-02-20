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
	: SGUIElement(parent)
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

SGUIButton* SGUIButton::create(SGUIGroup* parent, SGUIButtonInfo* btnInfo) {
	SGUIButton* pBtn = dbg_new SGUIButton(parent,btnInfo);
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

	return true;
}

void SGUIButton::load() { 
	if (m_bLoaded)
		return;

	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGUIManager* pUIManager = SGUIManager::get();
	SGImagePack* pPack = pPackManager->getPack(m_pInfo->Sga);

	m_pTexture[eNormal] = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Normal);
	m_pTexture[eNormal]->retain();
	m_pTexture[eOver] = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Over);
	m_pTexture[eOver]->retain();
	m_pTexture[ePressed] = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Pressed);
	m_pTexture[ePressed]->retain();
	m_pTexture[eDisabled] = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Disabled);
	m_pTexture[eDisabled]->retain();

	pUIManager->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Normal });
	pUIManager->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Over });
	pUIManager->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Pressed });
	pUIManager->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Disabled });

	DebugAssertMsg(!m_pTexture[eNormal]->isLink(), "버튼의 노말 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
	DebugAssertMsg(!m_pTexture[eOver]->isLink(), "버튼의 오버 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
	DebugAssertMsg(!m_pTexture[ePressed]->isLink(), "버튼의 프레스트 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");
	DebugAssertMsg(!m_pTexture[eDisabled]->isLink(), "버튼의 디세이블드 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");

	m_pSprite[eNormal] = Sprite::create();
	m_pSprite[eNormal]->initWithTexture(m_pTexture[eNormal]->getTexture());
	m_pSprite[eNormal]->setAnchorPoint(Vec2::ZERO);
	m_pSprite[eOver] = Sprite::create();
	m_pSprite[eOver]->initWithTexture(m_pTexture[eOver]->getTexture());
	m_pSprite[eOver]->setAnchorPoint(Vec2::ZERO);
	m_pSprite[ePressed] = Sprite::create();
	m_pSprite[ePressed]->initWithTexture(m_pTexture[ePressed]->getTexture());
	m_pSprite[ePressed]->setAnchorPoint(Vec2::ZERO);
	m_pSprite[eDisabled] = Sprite::create();
	m_pSprite[eDisabled]->initWithTexture(m_pTexture[eDisabled]->getTexture());
	m_pSprite[eDisabled]->setAnchorPoint(Vec2::ZERO);

	this->addChild(m_pSprite[eNormal]);
	this->addChild(m_pSprite[eOver]);
	this->addChild(m_pSprite[ePressed]);
	this->addChild(m_pSprite[eDisabled]);

	setVisibleState(eNormal);
	setContentSize(m_pSprite[eNormal]->getContentSize());

	m_bLoaded = true;
}

void SGUIButton::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	m_pSprite[eNormal] = nullptr;
	m_pSprite[eOver] = nullptr;
	m_pSprite[ePressed] = nullptr;
	m_pSprite[eDisabled] = nullptr;

	CC_SAFE_RELEASE_NULL(m_pTexture[eNormal]);
	CC_SAFE_RELEASE_NULL(m_pTexture[eOver]);
	CC_SAFE_RELEASE_NULL(m_pTexture[ePressed]);
	CC_SAFE_RELEASE_NULL(m_pTexture[eDisabled]);

	m_bLoaded = false;
}


bool SGUIButton::onMouseMove(SGEventMouse* mouseEvent) {
	if (m_eState == eDisabled ||
		m_eState == ePressed)
		return true;

	Vec2 mousePos = mouseEvent->getCursorPos();
	SGRect worldRect = getWorldBoundingBox();
	bool bContainedMouse = worldRect.containsPoint(mousePos);

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

	Vec2 mousePos = mouseEvent->getCursorPos();
	bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

	if (!bContainedMouse) {
		return true;
	}

	setVisibleState(ePressed);
	return false;
}

bool SGUIButton::onMouseUp(SGEventMouse* mouseEvent) {
	if (m_eState != ePressed)
		return true;

	Vec2 mousePos = mouseEvent->getCursorPos();
	bool bContainedMouse = getWorldBoundingBox().containsPoint(mousePos);

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

int SGUIButton::getCode() {
	return m_pInfo->Code;
}
 