/*
 * 작성자: 윤정도
 * 생성일: 2/23/2023 12:10:46 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "UICheckBox.h"
#include "GameCoreHeader.h"

#include <SteinsGate/Client/UIRootGroup.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox::UICheckBox(UIRootGroup* _pMaster, UIGroup* _pParent)
: UIElement(_pMaster, _pParent)
, pInfo_(nullptr)
, pTexture_{}
, pSprite_{}
, checked_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox::UICheckBox(UIRootGroup* _pMaster, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo, bool _infoOwner)
: UIElement(_pMaster, _pParent, _pCheckBoxInfo, _infoOwner)
, pInfo_(_pCheckBoxInfo)
, pTexture_{}
, pSprite_{}
, checked_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox::~UICheckBox()
{
	for (int i = 0; i < TextureCount; ++i)
	{
		CC_SAFE_RELEASE(pTexture_[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox* UICheckBox::create(UIRootGroup* _pMaster, UIGroup* _pParent)
{
	UICheckBox* pCheckBox = dbg_new UICheckBox(_pMaster, _pParent);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
UICheckBox* UICheckBox::create(UIRootGroup* _pMaster, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo,
                               bool _infoOwner)
{
	UICheckBox* pCheckBox = dbg_new UICheckBox(_pMaster, _pParent, _pCheckBoxInfo, _infoOwner);
	pCheckBox->init();
	pCheckBox->autorelease();
	return pCheckBox;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::setCheck(bool _checked)
{
	if (pSprite_[IndexCross])
		pSprite_[IndexCross]->setVisible(_checked);

	if (pSprite_[IndexCrossDisabled])
		pSprite_[IndexCrossDisabled]->setVisible(_checked);

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

		if (pSprite_[IndexBackground])
			pSprite_[IndexBackground]->setVisible(true);

		if (pSprite_[IndexBackgroundDisabled])
			pSprite_[IndexBackgroundDisabled]->setVisible(false);

		if (!checked_)
			return;

		if (pSprite_[IndexCross])
			pSprite_[IndexCross]->setVisible(true);

		if (pSprite_[IndexCrossDisabled])
			pSprite_[IndexCrossDisabled]->setVisible(false);

		state_ = eNormal;
		return;
	}

	if (pSprite_[IndexBackground])
		pSprite_[IndexBackground]->setVisible(false);

	if (pSprite_[IndexBackgroundDisabled])
		pSprite_[IndexBackgroundDisabled]->setVisible(true);

	if (checked_)
	{
		if (pSprite_[IndexCross])
			pSprite_[IndexCross]->setVisible(false);

		if (pSprite_[IndexCrossDisabled])
			pSprite_[IndexCrossDisabled]->setVisible(true);
	}

	state_ = eDisabled;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::SetUISize(const SGSize& _size)
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
	if (_pInfo->Type != UIElementType::CheckBox)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(pInfo_);
	}

	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UICheckBoxInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::setInfoCheckBox(UICheckBoxInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UICheckBox::isChecked() const
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

	const ImagePack* pBackgroundPack = Core::Contents.PackManager->getPackUnsafe(pInfo_->BackgroundSga);
	const ImagePack* pCrossPack = Core::Contents.PackManager->getPackUnsafe(pInfo_->CrossSga);
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
		pBackgroundSprite = pBackgroundPack->getSpriteUnsafe(pInfo_->BackgroundImg, pInfo_->Sprites[IndexBackground]);
		pBackgroundDisabledSprite = pBackgroundPack->getSpriteUnsafe(pInfo_->BackgroundImg,
		                                                             pInfo_->Sprites[IndexBackgroundDisabled]);
	}

	SgaSpriteAbstractPtr pCrossSprite = pCrossPack->getSpriteUnsafe(pInfo_->CrossImg, pInfo_->Sprites[IndexCross]);
	SgaSpriteAbstractPtr pCrossDisabledSprite = pCrossPack->getSpriteUnsafe(
		pInfo_->CrossImg, pInfo_->Sprites[IndexCrossDisabled]);

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

	const int sga[TextureCount]{ pInfo_->BackgroundSga, pInfo_->BackgroundSga, pInfo_->CrossSga, pInfo_->CrossSga };
	const int img[TextureCount]{ pInfo_->BackgroundImg, pInfo_->BackgroundImg, pInfo_->CrossImg, pInfo_->CrossImg };

	for (int i = 0; i < TextureCount; ++i)
	{
		const int spriteIndex = pInfo_->Sprites[i];

		if (spriteIndex == InvalidValue_v)
		{
			continue;
		}

		FrameTexture* pTexture = Core::Contents.UIManager->createUITextureRetained(sga[i], img[i], spriteIndex);
		Sprite* pSprite = Sprite::create();
		pSprite->initWithTexture(pTexture->getTexture());
		pSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		pSprite->setScaleX(uiSize_.width / pTexture->getWidthF());
		pSprite->setScaleY(uiSize_.height / pTexture->getHeightF());
		pSprite->setPosition(uiSize_ / 2);

		pTexture_[i] = pTexture;
		pSprite_[i] = pSprite;

		addChild(pSprite);
	}

	SetEnabled(true);
	setCheck(checked_);
	isLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UICheckBox::Unload()
{
	if (!isLoaded_)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < TextureCount; ++i)
	{
		pSprite_[i] = nullptr;
		CC_SAFE_RELEASE_NULL(pTexture_[i]);
	}

	isLoaded_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UICheckBox::OnMouseUpContainedInternalDetail(SGEventMouse* /*_pMouseEvent*/)
{
	if (state_ != eDisabled)
		setCheck(!checked_);

	return false;
}
