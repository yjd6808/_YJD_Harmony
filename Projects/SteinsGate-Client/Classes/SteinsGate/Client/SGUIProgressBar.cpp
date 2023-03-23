/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 3:16:12 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIProgressBar.h"

#include <SteinsGate/Client/SGUIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

SGUIProgressBar::SGUIProgressBar(SGUIMasterGroup* master, SGUIGroup* parent, SGUIProgressBarInfo* progressBarInfo)
	: SGUIElement(master, parent, progressBarInfo)
	, m_pInfo(progressBarInfo)
	, m_pTexture{}
	, m_pProgressBar{}
	, m_pProgressSprite(nullptr)
{}

SGUIProgressBar::~SGUIProgressBar() {
	CC_SAFE_RELEASE(m_pTexture);
}

SGUIProgressBar* SGUIProgressBar::create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIProgressBarInfo* progressBarInfo) {
	SGUIProgressBar* pProgressBar = dbg_new SGUIProgressBar(master, parent, progressBarInfo);
	pProgressBar->init();
	pProgressBar->autorelease();
	return pProgressBar;
}

bool SGUIProgressBar::init() {
	setContentSize(m_pInfo->Size);
	return true;
}

void SGUIProgressBar::load() {
	if (m_bLoaded)
		return;

	SGImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);

	if (pPack == nullptr)
		return;

	m_pTexture = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Sprite);
	m_pTexture->retain();

	if (m_pTexture->isLink()) {
		CC_SAFE_RELEASE_NULL(m_pTexture);
		return;
	}

	CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Sprite });

	const Size progressSpriteSize = m_pTexture->getSize();
	const float fScaleX = m_pInfo->Size.width / progressSpriteSize.width;
	const float fScaleY = m_pInfo->Size.height / progressSpriteSize.height;

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

void SGUIProgressBar::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	m_pProgressBar = nullptr;
	CC_SAFE_RELEASE_NULL(m_pTexture);
	m_bLoaded = false;

	init();
}

void SGUIProgressBar::setPercent(float percent) const {
	m_pProgressBar->setPercentage(percent);
}

float SGUIProgressBar::getPercent() const {
	return m_pProgressBar->getPercentage();
}




