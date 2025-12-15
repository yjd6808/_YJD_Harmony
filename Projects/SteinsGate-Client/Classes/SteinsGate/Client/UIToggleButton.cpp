/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 1:18:28 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIToggleButton.h"

#include <SteinsGate/Client/UIRootGroup.h>

USING_NS_CC;
USING_NS_JC;

UIToggleButton::UIToggleButton(UIRootGroup* _pMaster, UIGroup* _pParent)
: UIElement(_pMaster, _pParent)
, toggleState_(ToggleState::eNormal)
, info_(nullptr)
, texture_{}
, sprite_{}
{
}

UIToggleButton::UIToggleButton(UIRootGroup* _pMaster, UIGroup* _pParent, UIToggleButtonInfo* _pBtnInfo,
                               bool _infoOwner)
: UIElement(_pMaster, _pParent, _pBtnInfo, _infoOwner)
, toggleState_(ToggleState::eNormal)
, info_(_pBtnInfo)
, texture_{}
, sprite_{}
{
}

UIToggleButton::~UIToggleButton()
{
	CC_SAFE_RELEASE(texture_[StateOne][eNormal]);
	CC_SAFE_RELEASE(texture_[StateOne][eOver]);
	CC_SAFE_RELEASE(texture_[StateOne][ePressed]);
	CC_SAFE_RELEASE(texture_[StateOne][eDisabled]);

	CC_SAFE_RELEASE(texture_[StateTwo][eNormal]);
	CC_SAFE_RELEASE(texture_[StateTwo][eOver]);
	CC_SAFE_RELEASE(texture_[StateTwo][ePressed]);
	CC_SAFE_RELEASE(texture_[StateTwo][eDisabled]);
}

void UIToggleButton::setToggleState(ToggleState _state)
{
	toggleState_ = _state;
	setVisibleState(state_);
}

UIToggleButton* UIToggleButton::create(UIRootGroup* _pMaster, UIGroup* _pParent)
{
	UIToggleButton* pBtn = dbg_new UIToggleButton(_pMaster, _pParent);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

UIToggleButton* UIToggleButton::create(UIRootGroup* _pMaster, UIGroup* _pParent, UIToggleButtonInfo* _pBtnInfo,
                                       bool _infoOwner)
{
	UIToggleButton* pBtn = dbg_new UIToggleButton(_pMaster, _pParent, _pBtnInfo, _infoOwner);
	pBtn->init();
	pBtn->autorelease();
	return pBtn;
}

void UIToggleButton::setVisibleState(State _state)
{
	// 로딩안됬으면 모두 nullptr이므로 널포익 발생함
	if (!isLoaded_)
		return;

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < eMax; ++j)
			sprite_[i][j]->setVisible(false);

	sprite_[(int)toggleState_][_state]->setVisible(true);
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
	setVisibleState(eDisabled);
}

void UIToggleButton::SetUISize(const SGSize& _contentSize)
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
			FrameTexture* pTexture = texture_[i][j];
			Sprite* pSprite = sprite_[i][j];

			if (pTexture == nullptr || pSprite == nullptr)
			{
				continue;
			}

			pSprite->setScaleX(uiSize_.width / pTexture->getWidthF());
			pSprite->setScaleY(uiSize_.height / pTexture->getHeightF());
		}
	}
}

void UIToggleButton::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->Type != UIElementType::ToggleButton)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(info_);
	}

	pBaseInfo_ = _pInfo;
	info_ = static_cast<UIToggleButtonInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIToggleButton::setInfoToggleButton(UIToggleButtonInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UIToggleButton::RestoreState(State _state)
{
	if (state_ == eDisabled)
		return;

	if (state_ == _state)
		setVisibleState(eNormal);
}

bool UIToggleButton::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	const ImagePack* pPack = Core::Contents.PackManager->getPackUnsafe(info_->Sga);
	SetInitialUISize(DEFAULT_SIZE30);
	if (pPack == nullptr)
	{
		_LogWarn_("토글버튼 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteAbstractPtr spSprite = pPack->getSpriteUnsafe(info_->Img, info_->Sprites[StateOne][eNormal]);

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
			const int iSprite = info_->Sprites[i][j];
			FrameTexture* pTexture = Core::Contents.UIManager->createUITextureRetained(
				info_->Sga, info_->Img, iSprite, info_->LinearDodge);

			Sprite* pSprite = Sprite::create();
			pSprite->initWithTexture(pTexture->getTexture());
			pSprite->setAnchorPoint(Vec2::ZERO);
			pSprite->setScaleX(uiSize_.width / pTexture->getWidthF());
			pSprite->setScaleY(uiSize_.height / pTexture->getHeightF());

			texture_[i][j] = pTexture;
			sprite_[i][j] = pSprite;

			this->addChild(pSprite);
		}
	}


	isLoaded_ = true;
	setVisibleState(eNormal);
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
			sprite_[i][j] = nullptr;
			CC_SAFE_RELEASE_NULL(texture_[i][j]);
		}
	}

	isLoaded_ = false;
}

void UIToggleButton::OnMouseEnterInternalDetail(SGEventMouse* _pMouseEvent)
{
	setVisibleState(eOver);
}

void UIToggleButton::OnMouseLeaveInternalDetail(SGEventMouse* _pMouseEvent)
{
	setVisibleState(State::eNormal);
}

bool UIToggleButton::OnMouseMoveInternalDetail(SGEventMouse* _pMouseEvent)
{
	return true;
}

bool UIToggleButton::OnMouseDownInternalDetail(SGEventMouse* _pMouseEvent)
{
	setVisibleState(ePressed);
	return false;
}

void UIToggleButton::OnMouseUpInternalDetail(SGEventMouse* _pMouseEvent)
{
	setVisibleState(State::eNormal);
}

bool UIToggleButton::OnMouseUpContainedInternalDetail(SGEventMouse* _pMouseEvent)
{
	toggleState_ = toggleState_ == ToggleState::eNormal ? ToggleState::eToggled : ToggleState::eNormal;
	pRootGroup_->OnToggleStateChanged(this, toggleState_);
	setVisibleState(State::eNormal);
	return false;
}
