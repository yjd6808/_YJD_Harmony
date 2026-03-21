/*
 * 작성자: 윤정도
 * 생성일: 2/23/2023 12:10:46 AM
 * =====================
 *
 */

#include "UICheckBox.h"
#include "GameCoreHeader.h"

#include "sgcl/UIRootGroup.h"
#include "sgcl/UIManager.h"
#include "sgcl/ImagePackManager.h"

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox::UICheckBox(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, pInfo_(nullptr)
, pTexture_{}
, pSprite_{}
, checked_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox::UICheckBox(UIRootGroup* _pRoot, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo, bool _infoOwner)
: UIElement(_pRoot, _pParent, _pCheckBoxInfo, _infoOwner)
, pInfo_(_pCheckBoxInfo)
, pTexture_{}
, pSprite_{}
, checked_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox::~UICheckBox()
{
	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		CC_SAFE_RELEASE(pTexture_[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox* UICheckBox::Create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UICheckBox* pCheckBox = dbg_new UICheckBox(_pRoot, _pParent);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox* UICheckBox::Create(UIRootGroup* _pRoot, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo,
                               bool _infoOwner)
{
	UICheckBox* pCheckBox = dbg_new UICheckBox(_pRoot, _pParent, _pCheckBoxInfo, _infoOwner);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::SetEnabled(bool _enabled)
{
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

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::SetUISize(const cc::size& _size)
{
	if (!isResizable_)
		return;

	uiSize_ = _size;

	if (!isLoaded_)
		return;

	const float scaleX = getScaleX();
	const float scaleY = getScaleY();

	for (int i = 0; i < eMax; ++i)
	{
		Sprite* pSprite = pSprite_[i];

		if (pSprite == nullptr)
		{
			continue;
		}

		pSprite->setScaleX(scaleX);
		pSprite->setScaleY(scaleY);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::CheckBox)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JC_DELETE_SAFE(pInfo_);
	}

	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UICheckBoxInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::SetInfoCheckBox(UICheckBoxInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UICheckBox::IsChecked() const
{
	return checked_;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UICheckBox::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (pInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	const ImagePack* pBackgroundPack = g_cImagePackMgr.GetPackUnsafe(pInfo_->BackgroundSga);
	const ImagePack* pCrossPack = g_cImagePackMgr.GetPackUnsafe(pInfo_->CrossSga);
	SetInitialUISize(DEFAULT_SIZE30);
	checked_ = pInfo_->Check;

	// 백그라운드 팩은 없을 수도 있다. 크로스팩은 필수
	if (pCrossPack == nullptr)
	{
		_LogWarn_("체크박스 크로스 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	SgaSpriteAbstractPtr pBackgroundSprite;
	SgaSpriteAbstractPtr pBackgroundDisabledSprite;

	if (pBackgroundPack != nullptr)
	{
		pBackgroundSprite = pBackgroundPack->GetSpriteUnsafe(pInfo_->BackgroundImg, pInfo_->Sprites[INDEX_BACKGROUND]);
		pBackgroundDisabledSprite = pBackgroundPack->GetSpriteUnsafe(pInfo_->BackgroundImg,
		                                                             pInfo_->Sprites[INDEX_BACKGROUND_DISABLED]);
	}

	SgaSpriteAbstractPtr pCrossSprite = pCrossPack->GetSpriteUnsafe(pInfo_->CrossImg, pInfo_->Sprites[INDEX_CROSS]);
	SgaSpriteAbstractPtr pCrossDisabledSprite = pCrossPack->GetSpriteUnsafe(
		pInfo_->CrossImg, pInfo_->Sprites[INDEX_CROSS_DISABLED]);

	const float width = SgaSpriteHelper::GetMaxWidthF(pBackgroundSprite, pBackgroundDisabledSprite, pCrossSprite,
	                                                  pCrossDisabledSprite);
	const float height = SgaSpriteHelper::GetMaxHeightF(pBackgroundSprite, pBackgroundDisabledSprite, pCrossSprite,
	                                                    pCrossDisabledSprite);

	SetInitialUISize({ width, height });
	return isInitialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::Load()
{
	if (pInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return;
	}

	if (isLoaded_)
		return;

	const int sga[TEXTURE_COUNT]{ pInfo_->BackgroundSga, pInfo_->BackgroundSga, pInfo_->CrossSga, pInfo_->CrossSga };
	const int img[TEXTURE_COUNT]{ pInfo_->BackgroundImg, pInfo_->BackgroundImg, pInfo_->CrossImg, pInfo_->CrossImg };

	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		const int spriteIndex = pInfo_->Sprites[i];

		if (spriteIndex == InvalidValue_v)
		{
			continue;
		}

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
	}

	SetEnabled(true);
	SetCheck(checked_);
	isLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::Unload()
{
	if (!isLoaded_)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		pSprite_[i] = nullptr;
		CC_SAFE_RELEASE_NULL(pTexture_[i]);
	}

	isLoaded_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UICheckBox::OnMouseUpContainedInternalDetail(cc::EventMouse* /*_pMouseEvent*/)
{
	if (state_ != eDisabled)
		SetCheck(!checked_);

	return false;
}
