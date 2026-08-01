#include "GameCoreHeader.h"
#include "Game/UI/UIProgressBar.h"

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

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
		return false;

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
		return;

	if (LoadLegacy())
	{
		textureMode_ = UITextureMode::SGA;
	}
	else
	{
		_LogWarn_("레거시 스프라이트가 설정되지 않아 공용 UI 텍스처를 사용합니다.");
		textureMode_ = UITextureMode::THEME;
		LoadTheme();
	}

	isLoaded_ = true;
}

void UIProgressBar::LoadTheme()
{
	BuildThemeVisuals();
}

bool UIProgressBar::LoadLegacy()
{
	if (pProgressBarInfo_->Sprite == InvalidValue_v)
		return false;

	pTexture_ = g_cUIMgr.CreateUITextureRetained(pProgressBarInfo_->Sga, pProgressBarInfo_->Img, pProgressBarInfo_->Sprite);

	if (pTexture_->IsLink())
	{
		CC_SAFE_RELEASE_NULL(pTexture_);
		return false;
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
	return true;
}

void UIProgressBar::BuildThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();

	UIResolvedStyle resolved = pThemeMgr->Resolve(UIElementType::ProgressBar, UIVisualState::Normal, {});
	uint64_t hash = resolved.ComputeHash();

	auto* pTrack = Scale9Sprite::create();
	pTrack->setContentSize(uiSize_);
	pTrack->setAnchorPoint(Vec2::ZERO);
	this->addChild(pTrack);

	UIAssetKey trackKey = UIAssetKey::For(UIAssetSemantic::ProgressTrack, hash);
	themeBinding_.BindScale9(pTrack, trackKey, UIComponentSlot::Track);
	pTrackSprite_ = pTrack;

	auto* pGauge = Scale9Sprite::create();
	pGauge->setContentSize(uiSize_);
	pGauge->setAnchorPoint(Vec2::ZERO);
	pGauge->setVisible(false);
	this->addChild(pGauge);

	UIAssetKey gaugeKey = UIAssetKey::For(UIAssetSemantic::ProgressGauge, hash);
	themeBinding_.BindScale9(pGauge, gaugeKey, UIComponentSlot::Gauge);
	pGaugeSprite_ = pGauge;

	auto* pCap = Sprite::create();
	pCap->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pCap->setVisible(false);
	this->addChild(pCap);
	pGaugeCap_ = pCap;

	UIAssetKey capKey = UIAssetKey::For(UIAssetSemantic::ProgressCap, hash);
	themeBinding_.BindFixed(pCap, capKey, UIComponentSlot::Cap);

	_LogDebug_("[UIProgressBar] BuildThemeVisuals styleHash=%llu", hash);

	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);
	else
		_LogWarn_("[UIProgressBar] GetActiveTextureSet returned null!");

	pTrack->setContentSize(uiSize_);
	pGauge->setContentSize(uiSize_);

	const UITextureEntry* pTrackEntry = pSet ? pSet->Find(trackKey) : nullptr;
	gaugeInset_ = pTrackEntry ? pTrackEntry->contentPadding + resolved.geometryBorderWidth : 0.0f;
	if (gaugeInset_ < 0.0f)
		gaugeInset_ = 0.0f;
}

void UIProgressBar::DestroyThemeVisuals()
{
	themeBinding_.Clear();
	removeAllChildren();
	pTrackSprite_ = nullptr;
	pGaugeSprite_ = nullptr;
	pGaugeCap_ = nullptr;
}

void UIProgressBar::RefreshThemeVisuals()
{
	if (textureMode_ != UITextureMode::THEME || !pTrackSprite_)
		return;

	UIThemeManager* pThemeMgr = UIThemeManager::Get();
	UIResolvedStyle resolved = pThemeMgr->Resolve(UIElementType::ProgressBar, UIVisualState::Normal, {});
	uint64_t hash = resolved.ComputeHash();

	themeBinding_.Clear();
	themeBinding_.BindScale9(pTrackSprite_, UIAssetKey::For(UIAssetSemantic::ProgressTrack, hash), UIComponentSlot::Track);
	themeBinding_.BindScale9(pGaugeSprite_, UIAssetKey::For(UIAssetSemantic::ProgressGauge, hash), UIComponentSlot::Gauge);
	themeBinding_.BindFixed(pGaugeCap_, UIAssetKey::For(UIAssetSemantic::ProgressCap, hash), UIComponentSlot::Cap);

	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
	{
		themeBinding_.Refresh(*pSet);

		const UITextureEntry* pTrackEntry = pSet->Find(UIAssetKey::For(UIAssetSemantic::ProgressTrack, hash));
		gaugeInset_ = pTrackEntry ? pTrackEntry->contentPadding + resolved.geometryBorderWidth : 0.0f;
		if (gaugeInset_ < 0.0f)
			gaugeInset_ = 0.0f;
	}

	pTrackSprite_->setContentSize(uiSize_);
	pGaugeSprite_->setContentSize(uiSize_);
	SetPercent(percent_);
	_LogDebug_("[UIProgressBar] RefreshThemeVisuals name=%s styleHash=%llu gaugeInset=%.2f", GetName(), hash, gaugeInset_);
}

void UIProgressBar::Unload()
{
	if (!isLoaded_)
		return;

	removeAllChildren();

	if (textureMode_ == UITextureMode::THEME)
	{
		themeBinding_.Clear();
		pTrackSprite_ = nullptr;
		pGaugeSprite_ = nullptr;
		pGaugeCap_ = nullptr;
	}
	else
	{
		pProgressBar_ = nullptr;
		CC_SAFE_RELEASE_NULL(pTexture_);
	}

	isLoaded_ = false;
	isInitialized_ = false;
	init();
}

void UIProgressBar::SetUISize(const cc::size& _size)
{
	if (!isResizable_)
		return;

	uiSize_ = _size;

	if (!isLoaded_)
		return;

	if (textureMode_ == UITextureMode::THEME)
	{
		if (pTrackSprite_)
			pTrackSprite_->setContentSize(uiSize_);
		if (pGaugeSprite_)
			pGaugeSprite_->setContentSize(uiSize_);
		UpdateGaugeGeometry();
		return;
	}

	if (pProgressBar_ == nullptr)
		return;

	if (pTexture_ == nullptr)
		return;

	const Size spriteSize = pTexture_->GetSize();
	const float scaleX = uiSize_.width / spriteSize.width;
	const float scaleY = uiSize_.height / spriteSize.height;

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
		JC_DELETE_SAFE(pBaseInfo_);

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
	if (textureMode_ == UITextureMode::THEME)
	{
		if (pGaugeSprite_)
		{
			const float bodyW = uiSize_.width - gaugeInset_ * 2.0f;
			const float bodyH = uiSize_.height - gaugeInset_ * 2.0f;
			const float visibleWidth = bodyW * (_percent / 100.0f);
			percent_ = _percent;
			if (bodyW <= 0.0f || bodyH <= 0.0f || visibleWidth <= 0.01f)
			{
				pGaugeSprite_->setVisible(false);
				return;
			}
			pGaugeSprite_->setPosition(gaugeInset_, gaugeInset_);
			pGaugeSprite_->setVisible(true);
			pGaugeSprite_->setContentSize({ visibleWidth, bodyH });
		}
		return;
	}

	if (pProgressBar_)
		pProgressBar_->setPercentage(_percent);
}

float UIProgressBar::GetPercent() const
{
	if (pProgressBar_)
		return pProgressBar_->getPercentage();
	return 0.0f;
}

void UIProgressBar::UpdateGaugeGeometry()
{
	const float bodyW = uiSize_.width - gaugeInset_ * 2.0f;
	const float bodyH = uiSize_.height - gaugeInset_ * 2.0f;
	const float visibleWidth = bodyW * percent_;
	if (bodyW <= 0.0f || bodyH <= 0.0f || visibleWidth <= 0.01f)
	{
		if (pGaugeSprite_) pGaugeSprite_->setVisible(false);
		if (pGaugeCap_) pGaugeCap_->setVisible(false);
		return;
	}
	if (visibleWidth < gaugeMinSize_.width)
	{
		if (pGaugeSprite_) pGaugeSprite_->setVisible(false);
		if (pGaugeCap_)
		{
			pGaugeCap_->setVisible(true);
			pGaugeCap_->setPosition(visibleWidth * 0.5f + gaugeInset_, uiSize_.height * 0.5f);
			pGaugeCap_->setScale(1.0f);
		}
		return;
	}
	if (pGaugeCap_)
	{
		pGaugeCap_->setVisible(false);
	}
	if (pGaugeSprite_)
	{
		pGaugeSprite_->setPosition(gaugeInset_, gaugeInset_);
		pGaugeSprite_->setVisible(true);
		pGaugeSprite_->setContentSize({ visibleWidth, bodyH });
	}
}
