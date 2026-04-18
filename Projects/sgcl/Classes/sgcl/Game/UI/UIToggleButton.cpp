/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 1:18:28 PM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "Game/UI/UIToggleButton.h"

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/Texture/ImagePackManager.h"

USING_NS_CC;
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
	// 로딩안됬으면 모두 nullptr이므로 널포익 발생함
	if (!isLoaded_)
		return;

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < eMax; ++j)
			pSprites_[i][j]->setVisible(false);

	pSprites_[(int)toggleState_][_state]->setVisible(true);
}

void UIToggleButton::SetEnabled(bool _enabled)
{
	if (_enabled)
	{
		// 활성화 요청을 했지만 이미 활성화 상태인 경우는 무시
		if (state_ != eDisabled)
			return;

		// 이전 상태가 비활성화 상태 인경우 현재 상태가 Pressed인지, Moved인지 체크해서 업데이트
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

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < eMax; ++j)
		{
			FrameTexture* pTexture = pTextures_[i][j];
			Sprite* pSprite = pSprites_[i][j];

			if (pTexture == nullptr || pSprite == nullptr)
			{
				continue;
			}

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
	{
		JC_DELETE_SAFE(pInfo_);
	}

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

	if (state_ == _state)
		SetVisibleState(eNormal);
}

bool UIToggleButton::init()
{
	if (!UIElement::init())
	{
		return false;
	}

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


	isLoaded_ = true;
	SetVisibleState(eNormal);
}

void UIToggleButton::Unload()
{
	if (isLoaded_ == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < eMax; ++j)
		{
			pSprites_[i][j] = nullptr;
			CC_SAFE_RELEASE_NULL(pTextures_[i][j]);
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
