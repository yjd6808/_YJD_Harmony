/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 3:16:12 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIProgressBar.h"

#include <SteinsGate/Client/UIRootGroup.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIProgressBar::UIProgressBar(UIRootGroup* _pMasterGroup, UIGroup* _pParent)
: UIElement(_pMasterGroup, _pParent)
, progressBarInfo_(nullptr)
, texture_(nullptr)
, progressBar_(nullptr)
, progressSprite_(nullptr)
{
}

UIProgressBar::UIProgressBar(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UIProgressBarInfo* _pProgressBarInfo,
                             bool _infoOwner)
: UIElement(_pMasterGroup, _pParent, _pProgressBarInfo, _infoOwner)
, progressBarInfo_(_pProgressBarInfo)
, texture_(nullptr)
, progressBar_(nullptr)
, progressSprite_(nullptr)
{
}

UIProgressBar::~UIProgressBar()
{
	CC_SAFE_RELEASE(texture_);
}

UIProgressBar* UIProgressBar::create(UIRootGroup* _pMasterGroup, UIGroup* _pParent)
{
	UIProgressBar* pProgressBar = dbg_new UIProgressBar(_pMasterGroup, _pParent);
	pProgressBar->init();
	pProgressBar->autorelease();
	return pProgressBar;
}

UIProgressBar* UIProgressBar::create(UIRootGroup* _pMasterGroup, UIGroup* _pParent,
                                     UIProgressBarInfo* _pProgressBarInfo, bool _infoOwner)
{
	UIProgressBar* pProgressBar = dbg_new UIProgressBar(_pMasterGroup, _pParent, _pProgressBarInfo, _infoOwner);
	pProgressBar->init();
	pProgressBar->autorelease();
	return pProgressBar;
}

bool UIProgressBar::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (progressBarInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	SetInitialUISize(progressBarInfo_->Size);
	return isInitialized_ = true;
}

void UIProgressBar::Load()
{
	if (progressBarInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return;
	}

	if (isLoaded_)
	{
		return;
	}

	texture_ = Core::Contents.UIManager->createUITextureRetained(progressBarInfo_->Sga, progressBarInfo_->Img,
	                                                             progressBarInfo_->Sprite);

	if (texture_->isLink())
	{
		CC_SAFE_RELEASE_NULL(texture_);
		return;
	}

	const Size progressSpriteSize = texture_->getSize();
	const float scaleX = uiSize_.width / progressSpriteSize.width;
	const float scaleY = uiSize_.height / progressSpriteSize.height;

	progressSprite_ = Sprite::create();
	progressSprite_->initWithTexture(texture_->getTexture());

	progressBar_ = SGProgressTimer::create(progressSprite_);
	progressBar_->setPercentage(0);
	progressBar_->setType(ProgressTimer::Type::BAR);
	progressBar_->setScale(scaleX, scaleY);
	progressBar_->setAnchorPoint(Vec2::ZERO);

	switch (progressBarInfo_->ProgressIncreaseDirection)
	{
	case ProgressIncreaseDirection::LeftRight:
		progressBar_->setMidpoint({ 0, 0 });
		progressBar_->setBarChangeRate({ 1, 0 });
		break;
	case ProgressIncreaseDirection::RightLeft:
		progressBar_->setMidpoint({ 1, 0 });
		progressBar_->setBarChangeRate({ 1, 0 });
		break;
	case ProgressIncreaseDirection::TopBottom:
		progressBar_->setMidpoint({ 0, 1 });
		progressBar_->setBarChangeRate({ 0, 1 });
		break;
	case ProgressIncreaseDirection::BottomTop:
		progressBar_->setMidpoint({ 0, 0 });
		progressBar_->setBarChangeRate({ 0, 1 });
		break;
	}

	this->addChild(progressBar_);
	auto pProgressTo = ProgressTo::create(10.0f, 100);
	progressBar_->runAction(pProgressTo);
	isLoaded_ = true;
}

void UIProgressBar::Unload()
{
	if (!isLoaded_)
	{
		return;
	}

	removeAllChildren(); // autorelease 되기땜

	progressBar_ = nullptr;
	CC_SAFE_RELEASE_NULL(texture_);
	isLoaded_ = false;
	isInitialized_ = false;

	init();
}

void UIProgressBar::SetUISize(const SGSize& _size)
{
	if (!isResizable_)
	{
		return;
	}

	uiSize_ = _size;

	if (!isLoaded_)
	{
		return;
	}

	const float scaleX = getScaleX();
	const float scaleY = getScaleY();

	if (progressBar_ == nullptr)
	{
		return;
	}

	progressBar_->setScaleX(scaleX);
	progressBar_->setScaleY(scaleY);
}

void UIProgressBar::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->Type != UIElementType::ProgressBar)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(pBaseInfo_);
	}

	pBaseInfo_ = _pInfo;
	progressBarInfo_ = static_cast<UIProgressBarInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIProgressBar::setInfoProgressBar(UIProgressBarInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UIProgressBar::setPercent(float _percent) const
{
	progressBar_->setPercentage(_percent);
}

float UIProgressBar::getPercent() const
{
	return progressBar_->getPercentage();
}
