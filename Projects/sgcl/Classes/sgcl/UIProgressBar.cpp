/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 3:16:12 PM
 * =====================
 *
 */

#include "Core.h"
#include "GameCoreHeader.h"
#include "UIProgressBar.h"

#include <sgcl/UIRootGroup.h>
#include <sgcl/UIManager.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIProgressBar::UIProgressBar(UIRootGroup* _pMasterGroup, UIGroup* _pParent)
: UIElement(_pMasterGroup, _pParent)
, pProgressBarInfo_(nullptr)
, pTexture_(nullptr)
, pProgressBar_(nullptr)
, pProgressSprite_(nullptr)
{
}

UIProgressBar::UIProgressBar(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UIProgressBarInfo* _pProgressBarInfo,
                             bool _infoOwner)
: UIElement(_pMasterGroup, _pParent, _pProgressBarInfo, _infoOwner)
, pProgressBarInfo_(_pProgressBarInfo)
, pTexture_(nullptr)
, pProgressBar_(nullptr)
, pProgressSprite_(nullptr)
{
}

UIProgressBar::~UIProgressBar()
{
	CC_SAFE_RELEASE(pTexture_);
}

UIProgressBar* UIProgressBar::Create(UIRootGroup* _pMasterGroup, UIGroup* _pParent)
{
	UIProgressBar* pProgressBar = dbg_new UIProgressBar(_pMasterGroup, _pParent);
	pProgressBar->init();
	pProgressBar->autorelease();
	return pProgressBar;
}

UIProgressBar* UIProgressBar::Create(UIRootGroup* _pMasterGroup, UIGroup* _pParent,
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

	if (pProgressBarInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	SetInitialUISize(pProgressBarInfo_->Size);
	return isInitialized_ = true;
}

void UIProgressBar::Load()
{
	if (pProgressBarInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return;
	}

	if (isLoaded_)
	{
		return;
	}

	pTexture_ = g_cUIMgr.CreateUITextureRetained(pProgressBarInfo_->Sga, pProgressBarInfo_->Img, pProgressBarInfo_->Sprite);

	if (pTexture_->IsLink())
	{
		CC_SAFE_RELEASE_NULL(pTexture_);
		return;
	}

	const Size progressSpriteSize = pTexture_->GetSize();
	const float scaleX = uiSize_.width / progressSpriteSize.width;
	const float scaleY = uiSize_.height / progressSpriteSize.height;

	pProgressSprite_ = Sprite::create();
	pProgressSprite_->initWithTexture(pTexture_->GetTexture());

	pProgressBar_ = cc::ProgressTimer::create(pProgressSprite_);
	pProgressBar_->setPercentage(0);
	pProgressBar_->setType(ProgressTimer::Type::BAR);
	pProgressBar_->setScale(scaleX, scaleY);
	pProgressBar_->setAnchorPoint(Vec2::ZERO);

	switch (pProgressBarInfo_->ProgressIncreaseDirection)
	{
	case ProgressIncreaseDirection::LeftRight:
		pProgressBar_->setMidpoint({ 0, 0 });
		pProgressBar_->setBarChangeRate({ 1, 0 });
		break;
	case ProgressIncreaseDirection::RightLeft:
		pProgressBar_->setMidpoint({ 1, 0 });
		pProgressBar_->setBarChangeRate({ 1, 0 });
		break;
	case ProgressIncreaseDirection::TopBottom:
		pProgressBar_->setMidpoint({ 0, 1 });
		pProgressBar_->setBarChangeRate({ 0, 1 });
		break;
	case ProgressIncreaseDirection::BottomTop:
		pProgressBar_->setMidpoint({ 0, 0 });
		pProgressBar_->setBarChangeRate({ 0, 1 });
		break;
	}

	this->addChild(pProgressBar_);
	auto pProgressTo = ProgressTo::create(10.0f, 100);
	pProgressBar_->runAction(pProgressTo);
	isLoaded_ = true;
}

void UIProgressBar::Unload()
{
	if (!isLoaded_)
	{
		return;
	}

	removeAllChildren(); // autorelease 되기땜

	pProgressBar_ = nullptr;
	CC_SAFE_RELEASE_NULL(pTexture_);
	isLoaded_ = false;
	isInitialized_ = false;

	init();
}

void UIProgressBar::SetUISize(const cc::size& _size)
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

	if (pProgressBar_ == nullptr)
	{
		return;
	}

	pProgressBar_->setScaleX(scaleX);
	pProgressBar_->setScaleY(scaleY);
}

void UIProgressBar::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::ProgressBar)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JC_DELETE_SAFE(pBaseInfo_);
	}

	pBaseInfo_ = _pInfo;
	pProgressBarInfo_ = static_cast<UIProgressBarInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIProgressBar::SetInfoProgressBar(UIProgressBarInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UIProgressBar::SetPercent(float _percent) const
{
	pProgressBar_->setPercentage(_percent);
}

float UIProgressBar::GetPercent() const
{
	return pProgressBar_->getPercentage();
}
