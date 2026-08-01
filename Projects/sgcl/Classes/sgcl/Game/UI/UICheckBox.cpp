#include "Game/UI/UICheckBox.h"
#include "GameCoreHeader.h"

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/Texture/ImagePackManager.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

UICheckBox::UICheckBox(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, pInfo_(nullptr)
, pTexture_{}
, pSprite_{}
, checked_(false)
{
}

UICheckBox::UICheckBox(UIRootGroup* _pRoot, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo, bool _infoOwner)
: UIElement(_pRoot, _pParent, _pCheckBoxInfo, _infoOwner)
, pInfo_(_pCheckBoxInfo)
, pTexture_{}
, pSprite_{}
, checked_(false)
{
}

UICheckBox::~UICheckBox()
{
	for (int i = 0; i < TEXTURE_COUNT; ++i)
		CC_SAFE_RELEASE(pTexture_[i]);
}

UICheckBox* UICheckBox::Create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UICheckBox* pCheckBox = dbg_new UICheckBox(_pRoot, _pParent);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}

UICheckBox* UICheckBox::Create(UIRootGroup* _pRoot, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo, bool _infoOwner)
{
	UICheckBox* pCheckBox = dbg_new UICheckBox(_pRoot, _pParent, _pCheckBoxInfo, _infoOwner);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}

void UICheckBox::SetCheck(bool _checked)
{
	if (pSprite_[INDEX_CROSS])
		pSprite_[INDEX_CROSS]->setVisible(_checked);
	if (pSprite_[INDEX_CROSS_DISABLED])
		pSprite_[INDEX_CROSS_DISABLED]->setVisible(_checked);

	const bool prevChecked = checked_;
	checked_ = _checked;
	if (prevChecked != _checked)
		pRootGroup_->OnCheckedStateChanged(this, _checked);
}

void UICheckBox::SetEnabled(bool _enabled)
{
	if (textureMode_ == UITextureMode::THEME)
	{
		state_ = _enabled ? eNormal : eDisabled;
		setColor(_enabled ? Color3B::WHITE : Color3B(128, 128, 128));
		setOpacity(_enabled ? 255 : 128);
		return;
	}

	if (_enabled)
	{
		UpdateState();
		if (pSprite_[INDEX_BACKGROUND])
			pSprite_[INDEX_BACKGROUND]->setVisible(true);
		if (pSprite_[INDEX_BACKGROUND_DISABLED])
			pSprite_[INDEX_BACKGROUND_DISABLED]->setVisible(false);
		if (!checked_)
			return;
		if (pSprite_[INDEX_CROSS])
			pSprite_[INDEX_CROSS]->setVisible(true);
		if (pSprite_[INDEX_CROSS_DISABLED])
			pSprite_[INDEX_CROSS_DISABLED]->setVisible(false);
		state_ = eNormal;
		return;
	}

	if (pSprite_[INDEX_BACKGROUND])
		pSprite_[INDEX_BACKGROUND]->setVisible(false);
	if (pSprite_[INDEX_BACKGROUND_DISABLED])
		pSprite_[INDEX_BACKGROUND_DISABLED]->setVisible(true);
	if (checked_)
	{
		if (pSprite_[INDEX_CROSS])
			pSprite_[INDEX_CROSS]->setVisible(false);
		if (pSprite_[INDEX_CROSS_DISABLED])
			pSprite_[INDEX_CROSS_DISABLED]->setVisible(true);
	}
	state_ = eDisabled;
}

void UICheckBox::SetUISize(const cc::size& _size)
{
	if (!isResizable_)
		return;

	uiSize_ = _size;

	if (!isLoaded_)
		return;

	if (textureMode_ == UITextureMode::THEME)
	{
		if (pTrackShell_)
		{
			pTrackShell_->setContentSize(uiSize_);
			if (pSprite_[INDEX_CROSS])
				pSprite_[INDEX_CROSS]->setPosition(uiSize_ / 2);
		}
		return;
	}

	const float scaleX = getScaleX();
	const float scaleY = getScaleY();
	for (int i = 0; i < eMax; ++i)
	{
		Sprite* pSprite = pSprite_[i];
		if (pSprite == nullptr)
			continue;
		pSprite->setScaleX(scaleX);
		pSprite->setScaleY(scaleY);
	}
}

void UICheckBox::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::CheckBox)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}
	if (isInfoOwner_)
		JC_DELETE_SAFE(pInfo_);
	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UICheckBoxInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UICheckBox::SetInfoCheckBox(UICheckBoxInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

bool UICheckBox::IsChecked() const
{
	return checked_;
}

bool UICheckBox::init()
{
	if (!UIElement::init())
		return false;

	if (pInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	SetInitialUISize(DEFAULT_SIZE30);
	checked_ = pInfo_->Check;

	if (pInfo_->CrossSga != InvalidValue_v || pInfo_->BackgroundSga != InvalidValue_v)
	{
		const ImagePack* pBackgroundPack = pInfo_->BackgroundSga != InvalidValue_v
			? g_cImagePackMgr.GetPackUnsafe(pInfo_->BackgroundSga) : nullptr;
		const ImagePack* pCrossPack = pInfo_->CrossSga != InvalidValue_v
			? g_cImagePackMgr.GetPackUnsafe(pInfo_->CrossSga) : nullptr;

		SgaSpriteAbstractPtr pBackgroundSprite;
		SgaSpriteAbstractPtr pBackgroundDisabledSprite;
		SgaSpriteAbstractPtr pCrossSprite;
		SgaSpriteAbstractPtr pCrossDisabledSprite;

		if (pBackgroundPack)
		{
			pBackgroundSprite = pBackgroundPack->GetSpriteUnsafe(pInfo_->BackgroundImg, pInfo_->Sprites[INDEX_BACKGROUND]);
			pBackgroundDisabledSprite = pBackgroundPack->GetSpriteUnsafe(pInfo_->BackgroundImg, pInfo_->Sprites[INDEX_BACKGROUND_DISABLED]);
		}
		if (pCrossPack)
		{
			pCrossSprite = pCrossPack->GetSpriteUnsafe(pInfo_->CrossImg, pInfo_->Sprites[INDEX_CROSS]);
			pCrossDisabledSprite = pCrossPack->GetSpriteUnsafe(pInfo_->CrossImg, pInfo_->Sprites[INDEX_CROSS_DISABLED]);
		}

		const float width = SgaSpriteHelper::GetMaxWidthF(pBackgroundSprite, pBackgroundDisabledSprite, pCrossSprite, pCrossDisabledSprite);
		const float height = SgaSpriteHelper::GetMaxHeightF(pBackgroundSprite, pBackgroundDisabledSprite, pCrossSprite, pCrossDisabledSprite);

		if (width > 0 && height > 0)
			SetInitialUISize({ width, height });
	}

	return isInitialized_ = true;
}

void UICheckBox::Load()
{
	if (pInfo_ == nullptr)
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

void UICheckBox::LoadTheme()
{
	BuildThemeVisuals();
}

bool UICheckBox::LoadLegacy()
{
	bool bAnyLoaded = false;

	const int sga[TEXTURE_COUNT]{ pInfo_->BackgroundSga, pInfo_->BackgroundSga, pInfo_->CrossSga, pInfo_->CrossSga };
	const int img[TEXTURE_COUNT]{ pInfo_->BackgroundImg, pInfo_->BackgroundImg, pInfo_->CrossImg, pInfo_->CrossImg };

	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		const int spriteIndex = pInfo_->Sprites[i];
		if (spriteIndex == InvalidValue_v)
			continue;

		FrameTexture* pTexture = g_cUIMgr.CreateUITextureRetained(sga[i], img[i], spriteIndex);
		Sprite* pSprite = Sprite::create();
		pSprite->initWithTexture(pTexture->GetTexture());
		pSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		pSprite->setScaleX(uiSize_.width / pTexture->GetWidthF());
		pSprite->setScaleY(uiSize_.height / pTexture->GetHeightF());
		pSprite->setPosition(uiSize_ / 2);

		pTexture_[i] = pTexture;
		pSprite_[i] = pSprite;
		addChild(pSprite);
		bAnyLoaded = true;
	}

	SetEnabled(true);
	SetCheck(checked_);
	return bAnyLoaded;
}

void UICheckBox::BuildThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();

	auto* pTrack = Scale9Sprite::create();
	pTrack->setContentSize(uiSize_);
	pTrack->setAnchorPoint(Vec2::ZERO);
	this->addChild(pTrack);
	pTrackShell_ = pTrack;

	UIResolvedStyle trackStyle = pThemeMgr->Resolve(UIElementType::CheckBox, UIVisualState::Normal, {});

	UIAssetKey trackKey = UIAssetKey::For(UIAssetSemantic::CheckBox, trackStyle.ComputeHash());
	themeBinding_.BindScale9(pTrack, trackKey, UIComponentSlot::Shell);

	auto* pMark = Sprite::create();
	pMark->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pMark->setPosition(uiSize_ / 2);
	pMark->setVisible(checked_);
	this->addChild(pMark);

	UIAssetKey markKey = UIAssetKey::For(UIAssetSemantic::CheckMark, trackStyle.ComputeHash());
	themeBinding_.BindFixed(pMark, markKey, UIComponentSlot::Mark);

	pSprite_[INDEX_CROSS] = pMark;

	_LogDebug_("[UICheckBox] BuildThemeVisuals trackStyleHash=%llu markSemantic=CheckMark", trackKey.styleHash);

	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);
	else
		_LogWarn_("[UICheckBox] GetActiveTextureSet returned null!");

	pTrack->setContentSize(uiSize_);
}

void UICheckBox::DestroyThemeVisuals()
{
	themeBinding_.Clear();
	removeAllChildren();
	pTrackShell_ = nullptr;
}

void UICheckBox::RefreshThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();
	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);
}

void UICheckBox::Unload()
{
	if (!isLoaded_)
		return;

	removeAllChildren();

	if (textureMode_ == UITextureMode::THEME)
	{
		themeBinding_.Clear();
		pTrackShell_ = nullptr;
	}
	else
	{
		for (int i = 0; i < TEXTURE_COUNT; ++i)
		{
			pSprite_[i] = nullptr;
			CC_SAFE_RELEASE_NULL(pTexture_[i]);
		}
	}

	isLoaded_ = false;
}

bool UICheckBox::OnMouseUpContainedInternalDetail(cc::EventMouse* /*_pMouseEvent*/)
{
	if (state_ != eDisabled)
		SetCheck(!checked_);
	return false;
}
