/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 3:16:12 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIProgressBar.h"

#include <SteinsGate/Client/UIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

UIProgressBar::UIProgressBar(UIMasterGroup* master, UIGroup* parent, UIProgressBarInfo* progressBarInfo)
	: UIElement(master, parent, progressBarInfo)
	, m_pInfo(progressBarInfo)
	, m_pTexture{}
	, m_pProgressBar{}
	, m_pProgressSprite(nullptr)
{}

UIProgressBar::~UIProgressBar() {
	CC_SAFE_RELEASE(m_pTexture);
}

UIProgressBar* UIProgressBar::create(UIMasterGroup* master, UIGroup* parent, UIProgressBarInfo* progressBarInfo) {
	UIProgressBar* pProgressBar = dbg_new UIProgressBar(master, parent, progressBarInfo);
	pProgressBar->init();
	pProgressBar->autorelease();
	return pProgressBar;
}

bool UIProgressBar::init() {
	setInitialUISize(m_pInfo->Size);
	return m_bInitialized = true;
}

void UIProgressBar::load() {
	if (m_bLoaded)
		return;

	m_pTexture = CoreUIManager_v->createUITextureRetained(m_pInfo->Sga, m_pInfo->Img, m_pInfo->Sprite);

	if (m_pTexture->isLink()) {
		CC_SAFE_RELEASE_NULL(m_pTexture);
		return;
	}

	const Size progressSpriteSize = m_pTexture->getSize();
	const float fScaleX = m_UISize.width / progressSpriteSize.width;
	const float fScaleY = m_UISize.height / progressSpriteSize.height;

	m_pProgressSprite = Sprite::create();
	m_pProgressSprite->initWithTexture(m_pTexture->getTexture());

	m_pProgressBar = SGProgressTimer::create(m_pProgressSprite);
	m_pProgressBar->setPercentage(0);
	m_pProgressBar->setType(ProgressTimer::Type::BAR);
	m_pProgressBar->setScale(fScaleX, fScaleY);
	m_pProgressBar->setAnchorPoint(Vec2::ZERO);

	switch (m_pInfo->ProgressIncreaseDirection) {
	case ProgressIncreaseDirection::LeftRight:
		m_pProgressBar->setMidpoint({ 0, 0 });
		m_pProgressBar->setBarChangeRate({ 1, 0 });
		break;
	case ProgressIncreaseDirection::RightLeft:
		m_pProgressBar->setMidpoint({ 1, 0 });
		m_pProgressBar->setBarChangeRate({ 1, 0 });
		break;
	case ProgressIncreaseDirection::TopBottom:
		m_pProgressBar->setMidpoint({ 0, 1 });
		m_pProgressBar->setBarChangeRate({ 0, 1 });
		break;
	case ProgressIncreaseDirection::BottomTop:
		m_pProgressBar->setMidpoint({ 0, 0 });
		m_pProgressBar->setBarChangeRate({ 0, 1 });
		break;
	}

	this->addChild(m_pProgressBar);
	auto progressTo = ProgressTo::create(10.0f, 100);
	m_pProgressBar->runAction(progressTo);
	m_bLoaded = true;
}

void UIProgressBar::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	m_pProgressBar = nullptr;
	CC_SAFE_RELEASE_NULL(m_pTexture);
	m_bLoaded = false;
	m_bInitialized = false;

	init();
}

void UIProgressBar::setUISize(const SGSize& size) {
	if (!m_bResizable)
		return;

	m_UISize = size;

	if (!m_bLoaded)
		return;

	const float fScaleX = getScaleX();
	const float fScaleY = getScaleY();

	if (m_pProgressBar == nullptr)
		return;

	m_pProgressBar->setScaleX(fScaleX);
	m_pProgressBar->setScaleY(fScaleY);
}

void UIProgressBar::setPercent(float percent) const {
	m_pProgressBar->setPercentage(percent);
}

float UIProgressBar::getPercent() const {
	return m_pProgressBar->getPercentage();
}




