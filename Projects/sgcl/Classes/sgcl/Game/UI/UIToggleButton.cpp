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
		if (themeKnob_)
		{
			const float knobSize = jc::Math::Min(uiSize_.width, uiSize_.height) * 0.8f;
			const float xPos = (_state == ToggleState::eToggled) ? (uiSize_.width - knobSize * 0.5f) : (knobSize * 0.5f);
			themeKnob_->setPositionX(xPos);
		}
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
		ApplyThemeStateVisuals(_state);
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
	{
		if (themeTrack_)
		{
			auto* pTrack9 = dynamic_cast<Scale9Sprite*>(themeTrack_);
			if (pTrack9)
				pTrack9->setContentSize(uiSize_);
		}
		return;
	}

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

	SetInitialUISize(DEFAULT_SIZE30);

	if (pInfo_->Sga != InvalidValue_v)
	{
		const ImagePack* pPack = g_cImagePackMgr.GetPackUnsafe(pInfo_->Sga);
		if (pPack)
		{
			const SgaSpriteAbstractPtr spSprite = pPack->GetSpriteUnsafe(pInfo_->Img, pInfo_->Sprites[STATE_ONE][eNormal]);
			if (spSprite)
			{
				const SgaSpriteRect spriteRect = spSprite->GetRect();
				SetInitialUISize({ spriteRect.GetWidthF(), spriteRect.GetHeightF() });
			}
		}
	}

	return isInitialized_ = true;
}

void UIToggleButton::Load()
{
	if (isLoaded_)
		return;

	if (pBaseInfo_ && pBaseInfo_->renderMode_ != eRenderModeAuto)
		renderMode_ = (UIRenderMode)pBaseInfo_->renderMode_;

	if (UseThemeRendering())
	{
		LoadTheme();
		ApplyThemeStateVisuals(eNormal);
	}
	else
	{
		LoadLegacy();
	}

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
	themeTrack_ = pTrack;

	UIAssetKey trackKey;
	trackKey.semantic = UIAssetSemantic::ToggleTrack;
	themeBinding_.BindScale9(pTrack, trackKey, UIComponentSlot::Track);

	float knobSize = jc::Math::Min(uiSize_.width, uiSize_.height) * 0.8f;
	auto* pKnob = Sprite::create();
	pKnob->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	pKnob->setPosition(knobSize * 0.5f, uiSize_.height * 0.5f);
	this->addChild(pKnob);
	themeKnob_ = pKnob;

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
	themeTrack_ = nullptr;
	themeKnob_ = nullptr;
}

void UIToggleButton::ApplyThemeStateVisuals(State _state)
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

void UIToggleButton::RefreshThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();
	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);
}

void UIToggleButton::Unload()
{
	if (isLoaded_ == false)
		return;

	removeAllChildren();

	if (UseThemeRendering())
	{
		themeBinding_.Clear();
		themeTrack_ = nullptr;
		themeKnob_ = nullptr;
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
