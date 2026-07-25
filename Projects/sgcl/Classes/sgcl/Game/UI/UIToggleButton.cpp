#include "GameCoreHeader.h"
#include "Game/UI/UIToggleButton.h"

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/Texture/ImagePackManager.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

UIToggleButton::UIToggleButton(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, toggleState_(ToggleState::eNormal)
, pInfo_(nullptr)
, pTextures_{}
, pSprites_{}
{
}

UIToggleButton::UIToggleButton(UIRootGroup* _pRoot, UIGroup* _pParent, UIToggleButtonInfo* _pBtnInfo,
                               bool _infoOwner)
: UIElement(_pRoot, _pParent, _pBtnInfo, _infoOwner)
, toggleState_(ToggleState::eNormal)
, pInfo_(_pBtnInfo)
, pTextures_{}
, pSprites_{}
{
}

UIToggleButton::~UIToggleButton()
{
	CC_SAFE_RELEASE(pTextures_[STATE_ONE][eNormal]);
	CC_SAFE_RELEASE(pTextures_[STATE_ONE][eOver]);
	CC_SAFE_RELEASE(pTextures_[STATE_ONE][ePressed]);
	CC_SAFE_RELEASE(pTextures_[STATE_ONE][eDisabled]);
	CC_SAFE_RELEASE(pTextures_[STATE_TWO][eNormal]);
	CC_SAFE_RELEASE(pTextures_[STATE_TWO][eOver]);
	CC_SAFE_RELEASE(pTextures_[STATE_TWO][ePressed]);
	CC_SAFE_RELEASE(pTextures_[STATE_TWO][eDisabled]);
}

void UIToggleButton::SetToggleState(ToggleState _state)
{
	toggleState_ = _state;

	if (UseThemeRendering())
	{
		return;
	}

	SetVisibleState(state_);
}

UIToggleButton* UIToggleButton::Create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UIToggleButton* pBtn = dbg_new UIToggleButton(_pRoot, _pParent);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

UIToggleButton* UIToggleButton::Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIToggleButtonInfo* _pBtnInfo,
                                       bool _infoOwner)
{
	UIToggleButton* pBtn = dbg_new UIToggleButton(_pRoot, _pParent, _pBtnInfo, _infoOwner);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

void UIToggleButton::SetVisibleState(State _state)
{
	if (!isLoaded_)
		return;

	if (UseThemeRendering())
	{
		state_ = _state;
		return;
	}

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < eMax; ++j)
			pSprites_[i][j]->setVisible(false);

	pSprites_[(int)toggleState_][_state]->setVisible(true);
}

void UIToggleButton::SetEnabled(bool _enabled)
{
	if (_enabled)
	{
		if (state_ != eDisabled)
			return;
		UpdateState();
		return;
	}
	state_ = eDisabled;
	SetVisibleState(eDisabled);
}

void UIToggleButton::SetUISize(const cc::size& _contentSize)
{
	if (!isResizable_)
		return;

	uiSize_ = _contentSize;

	if (!isLoaded_)
		return;

	if (UseThemeRendering())
		return;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < eMax; ++j)
		{
			FrameTexture* pTexture = pTextures_[i][j];
			Sprite* pSprite = pSprites_[i][j];
			if (pTexture == nullptr || pSprite == nullptr)
				continue;
			pSprite->setScaleX(uiSize_.width / pTexture->GetWidthF());
			pSprite->setScaleY(uiSize_.height / pTexture->GetHeightF());
		}
	}
}

void UIToggleButton::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::ToggleButton)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}
	if (isInfoOwner_)
		JC_DELETE_SAFE(pInfo_);
	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UIToggleButtonInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIToggleButton::SetInfoToggleButton(UIToggleButtonInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UIToggleButton::RestoreState(State _state)
{
	if (state_ == eDisabled)
		return;

	SetVisibleState(_state);
}

bool UIToggleButton::init()
{
	if (!UIElement::init())
		return false;

	const ImagePack* pPack = g_cImagePackMgr.GetPackUnsafe(pInfo_->Sga);
	SetInitialUISize(DEFAULT_SIZE30);
	if (pPack == nullptr)
	{
		_LogWarn_("토글버튼 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteAbstractPtr spSprite = pPack->GetSpriteUnsafe(pInfo_->Img, pInfo_->Sprites[STATE_ONE][eNormal]);
	if (spSprite == nullptr)
	{
		_LogWarn_("토글버튼 노말 스프라이트를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteRect spriteRect = spSprite->GetRect();
	SetInitialUISize({ spriteRect.GetWidthF(), spriteRect.GetHeightF() });
	return isInitialized_ = true;
}

void UIToggleButton::Load()
{
	if (isLoaded_)
		return;

	if (pBaseInfo_ && pBaseInfo_->renderMode_ != eRenderModeAuto)
		renderMode_ = (UIRenderMode)pBaseInfo_->renderMode_;

	if (UseThemeRendering())
		LoadTheme();
	else
		LoadLegacy();

	isLoaded_ = true;
	SetVisibleState(eNormal);
}

void UIToggleButton::LoadTheme()
{
	BuildThemeVisuals();
}

void UIToggleButton::LoadLegacy()
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < eMax; ++j)
		{
			const int iSprite = pInfo_->Sprites[i][j];
			FrameTexture* pTexture = g_cUIMgr.CreateUITextureRetained(
				pInfo_->Sga, pInfo_->Img, iSprite, pInfo_->LinearDodge);

			Sprite* pSprite = Sprite::create();
			pSprite->initWithTexture(pTexture->GetTexture());
			pSprite->setAnchorPoint(Vec2::ZERO);
			pSprite->setScaleX(uiSize_.width / pTexture->GetWidthF());
			pSprite->setScaleY(uiSize_.height / pTexture->GetHeightF());

			pTextures_[i][j] = pTexture;
			pSprites_[i][j] = pSprite;

			this->addChild(pSprite);
		}
	}
}

void UIToggleButton::BuildThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();

	auto* pTrack = Scale9Sprite::create();
	pTrack->setContentSize(uiSize_);
	pTrack->setAnchorPoint(Vec2::ZERO);
	this->addChild(pTrack);

	UIAssetKey trackKey;
	trackKey.semantic = UIAssetSemantic::ToggleTrack;
	themeBinding_.BindScale9(pTrack, trackKey, UIComponentSlot::Track);

	float knobSize = jc::Math::Min(uiSize_.width, uiSize_.height) * 0.8f;
	auto* pKnob = Sprite::create();
	pKnob->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pKnob->setPosition(knobSize * 0.5f, uiSize_.height * 0.5f);
	this->addChild(pKnob);

	UIAssetKey knobKey;
	knobKey.semantic = UIAssetSemantic::ToggleKnob;
	themeBinding_.BindFixed(pKnob, knobKey, UIComponentSlot::Knob);

	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);
}

void UIToggleButton::DestroyThemeVisuals()
{
	themeBinding_.Clear();
	removeAllChildren();
}

void UIToggleButton::Unload()
{
	if (isLoaded_ == false)
		return;

	removeAllChildren();

	if (UseThemeRendering())
	{
		themeBinding_.Clear();
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < eMax; ++j)
			{
				pSprites_[i][j] = nullptr;
				CC_SAFE_RELEASE_NULL(pTextures_[i][j]);
			}
		}
	}

	isLoaded_ = false;
}

void UIToggleButton::OnMouseEnterInternalDetail(cc::EventMouse* _pMouseEvent)
{
	SetVisibleState(eOver);
}

void UIToggleButton::OnMouseLeaveInternalDetail(cc::EventMouse* _pMouseEvent)
{
	SetVisibleState(State::eNormal);
}

bool UIToggleButton::OnMouseMoveInternalDetail(cc::EventMouse* _pMouseEvent)
{
	return true;
}

bool UIToggleButton::OnMouseDownInternalDetail(cc::EventMouse* _pMouseEvent)
{
	SetVisibleState(ePressed);
	return false;
}

void UIToggleButton::OnMouseUpInternalDetail(cc::EventMouse* _pMouseEvent)
{
	SetVisibleState(State::eNormal);
}

bool UIToggleButton::OnMouseUpContainedInternalDetail(cc::EventMouse* _pMouseEvent)
{
	toggleState_ = toggleState_ == ToggleState::eNormal ? ToggleState::eToggled : ToggleState::eNormal;
	pRootGroup_->OnToggleStateChanged(this, toggleState_);
	SetVisibleState(State::eNormal);
	return false;
}
