/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:55:50 PM
 * =====================
 *
 */

#include "GameCoreHeader.h"
#include "Game/UI/UIButton.h"

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/Texture/ImagePackManager.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIButton::UIButton(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, buttonInfo_(nullptr)
, texture_{}
, sprite_{}
{
}

UIButton::UIButton(UIRootGroup* _pRoot, UIGroup* _pParent, UIButtonInfo* _pButtonInfo, bool _infoOwner)
: UIElement(_pRoot, _pParent, _pButtonInfo, _infoOwner)
, buttonInfo_(_pButtonInfo)
, texture_{}
, sprite_{}
{
}

UIButton::~UIButton()
{
	CC_SAFE_RELEASE(texture_[eNormal]);
	CC_SAFE_RELEASE(texture_[eOver]);
	CC_SAFE_RELEASE(texture_[ePressed]);
	CC_SAFE_RELEASE(texture_[eDisabled]);
}

UIButton* UIButton::Create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UIButton* pButton = dbg_new UIButton(_pRoot, _pParent);
	pButton->init();
	pButton->autorelease();
	return pButton;
}

UIButton* UIButton::Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIButtonInfo* _pButtonInfo, bool _infoOwner)
{
	UIButton* pButton = dbg_new UIButton(_pRoot, _pParent, _pButtonInfo, _infoOwner);
	pButton->init();
	pButton->autorelease();
	return pButton;
}

void UIButton::SetVisibleState(State _state)
{
	if (UseThemeRendering())
	{
		state_ = _state;
		ApplyThemeStateVisuals(_state);
		return;
	}

	for (int i = 0; i < eMax; ++i)
	{
		Sprite* pSprite = sprite_[i];

		if (pSprite == nullptr)
		{
			continue;
		}

		if (i == _state)
		{
			pSprite->setVisible(true);
			continue;
		}

		pSprite->setVisible(false);
	}

	state_ = _state;
}

void UIButton::SetUISize(const cc::size& _size)
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

	if (UseThemeRendering())
	{
		if (themeRoot_)
		{
			auto* pTrack = dynamic_cast<cc_ui::Scale9Sprite*>(themeRoot_);
			if (pTrack) pTrack->setContentSize(uiSize_);
		}
		return;
	}

	const float scaleX = getScaleX();
	const float scaleY = getScaleY();

	for (int i = 0; i < eMax; ++i)
	{
		Sprite* pSprite = sprite_[i];

		if (pSprite == nullptr)
		{
			continue;
		}

		pSprite->setScaleX(scaleX);
		pSprite->setScaleY(scaleY);
	}
}

void UIButton::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::Button)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JC_DELETE_SAFE(buttonInfo_);
	}

	pBaseInfo_ = _pInfo;
	buttonInfo_ = static_cast<UIButtonInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIButton::SetInfoButton(UIButtonInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UIButton::SetEnabled(bool _enabled)
{
	if (_enabled)
	{
		// 활성화 요청을 했지만 이미 활성화 상태인 경우는 무시
		if (state_ != eDisabled)
		{
			return;
		}

		// 이전 상태가 비활성화 상태 인경우 현재 상태가 Pressed인지, Moved인지 체크해서 업데이트
		UpdateState();
		SetVisibleState(eNormal);
		return;
	}

	state_ = eDisabled;
	SetVisibleState(eDisabled);
}

void UIButton::RestoreState(State _state)
{
	if (state_ == eDisabled)
	{
		return;
	}

	SetVisibleState(_state);
}

void UIButton::OnMouseEnterInternalDetail(cc::EventMouse* _pMouseEvent)
{
	SetVisibleState(eOver);
}

void UIButton::OnMouseLeaveInternalDetail(cc::EventMouse* _pMouseEvent)
{
	SetVisibleState(eNormal);
}

bool UIButton::OnMouseMoveInternalDetail(cc::EventMouse* _pMouseEvent)
{
	return false;
}

bool UIButton::OnMouseDownInternalDetail(cc::EventMouse* _pMouseEvent)
{
	SetVisibleState(ePressed);
	return false;
}

void UIButton::OnMouseUpInternalDetail(cc::EventMouse* _pMouseEvent)
{
	SetVisibleState(eNormal);
}

bool UIButton::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (buttonInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	SetInitialUISize(DEFAULT_SIZE30);

	if (buttonInfo_->sga_ != InvalidValue_v)
	{
		const ImagePack* pPack = g_cImagePackMgr.GetPackUnsafe(buttonInfo_->sga_);
		if (pPack)
		{
			const SgaSpriteAbstractPtr pSprite = pPack->GetSpriteUnsafe(buttonInfo_->img_, buttonInfo_->sprites_[eNormal]);
			if (pSprite != nullptr)
			{
				const SgaSpriteRect spriteRect = pSprite->GetRect();
				SetInitialUISize({ spriteRect.GetWidthF(), spriteRect.GetHeightF() });
			}
		}
	}

	return isInitialized_ = true;
}

void UIButton::Load()
{
	if (isLoaded_)
	{
		return;
	}

	if (pBaseInfo_ && pBaseInfo_->renderMode_ != eRenderModeAuto)
		renderMode_ = (UIRenderMode)pBaseInfo_->renderMode_;

	if (UseThemeRendering())
	{
		LoadTheme();
		ApplyThemeStateVisuals(eNormal);
	}
	else
	{
		if (!LoadLegacy())
		{
			_LogWarn_("레거시 스프라이트가 설정되지 않아 공용 UI 텍스처를 사용합니다.");
			renderMode_ = UIRenderMode::Theme;
			LoadTheme();
			ApplyThemeStateVisuals(eNormal);
		}
	}

	SetVisibleState(eNormal);
	isLoaded_ = true;
}

bool UIButton::LoadLegacy()
{
	bool bAnyLoaded = false;

	for (int i = 0; i < eMax; ++i)
	{
		const int spriteIndex = buttonInfo_->sprites_[i];

		if (spriteIndex == InvalidValue_v)
		{
			_LogWarn_("설정되지 않은 스프라이트입니다.");
			continue;
		}

		FrameTexture* pTexture = g_cUIMgr.CreateUITextureRetained(buttonInfo_->sga_, buttonInfo_->img_, spriteIndex, buttonInfo_->linearDodge_);

		Sprite* pSprite = Sprite::create();
		pSprite->initWithTexture(pTexture->GetTexture());
		pSprite->setScaleX(uiSize_.width / pTexture->GetWidthF());
		pSprite->setScaleY(uiSize_.height / pTexture->GetHeightF());
		pSprite->setAnchorPoint(Vec2::ZERO);

		texture_[i] = pTexture;
		sprite_[i] = pSprite;

		this->addChild(pSprite);
		bAnyLoaded = true;
	}

	return bAnyLoaded;
}

void UIButton::LoadTheme()
{
	BuildThemeVisuals();
}

void UIButton::BuildThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();

	auto* pTrack = Scale9Sprite::create();
	pTrack->setContentSize(uiSize_);
	pTrack->setAnchorPoint(Vec2::ZERO);
	this->addChild(pTrack);
	themeRoot_ = pTrack;

	UIAssetKey key;
	key.semantic = UIAssetSemantic::Button;
	key.styleHash = 0;
	key.recipeHash = 0;

	themeBinding_.BindScale9(pTrack, key, UIComponentSlot::Background);

	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);
}

void UIButton::DestroyThemeVisuals()
{
	themeBinding_.Clear();
	removeAllChildren();
	themeRoot_ = nullptr;
}

void UIButton::ApplyThemeStateVisuals(State _state)
{
	switch (_state)
	{
	case eNormal:
		setColor(Color3B::WHITE);
		setOpacity(255);
		break;
	case eOver:
		setColor(Color3B(240, 245, 255));
		setOpacity(255);
		break;
	case ePressed:
		setColor(Color3B(180, 180, 200));
		setOpacity(255);
		break;
	case eDisabled:
		setColor(Color3B(128, 128, 128));
		setOpacity(128);
		break;
	}
}

void UIButton::RefreshThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();
	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);
}

void UIButton::Unload()
{
	if (!isLoaded_)
	{
		return;
	}

	removeAllChildren();

	if (UseThemeRendering())
	{
		themeBinding_.Clear();
		themeRoot_ = nullptr;
	}
	else
	{
		for (int i = 0; i < eMax; ++i)
		{
			sprite_[i] = nullptr;
			CC_SAFE_RELEASE_NULL(texture_[i]);
		}
	}

	isLoaded_ = false;
}
