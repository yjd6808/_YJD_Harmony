/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:55:50 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIButton.h"

#include <SteinsGate/Client/UIRootGroup.h>

USING_NS_CC;
USING_NS_JC;

UIButton::UIButton(UIRootGroup* _pMaster, UIGroup* _pParent)
: UIElement(_pMaster, _pParent)
, buttonInfo_(nullptr)
, texture_{}
, sprite_{}
{
}

UIButton::UIButton(UIRootGroup* _pMaster, UIGroup* _pParent, UIButtonInfo* _pButtonInfo, bool _infoOwner)
: UIElement(_pMaster, _pParent, _pButtonInfo, _infoOwner)
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

UIButton* UIButton::create(UIRootGroup* _pMaster, UIGroup* _pParent)
{
	UIButton* pButton = dbg_new UIButton(_pMaster, _pParent);
	pButton->init();
	pButton->autorelease();
	return pButton;
}

UIButton* UIButton::create(UIRootGroup* _pMaster, UIGroup* _pParent, UIButtonInfo* _pButtonInfo, bool _infoOwner)
{
	UIButton* pButton = dbg_new UIButton(_pMaster, _pParent, _pButtonInfo, _infoOwner);
	pButton->init();
	pButton->autorelease();
	return pButton;
}

void UIButton::setVisibleState(State _state)
{
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

void UIButton::SetUISize(const SGSize& _size)
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
	if (_pInfo->Type != UIElementType::Button)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(buttonInfo_);
	}

	pBaseInfo_ = _pInfo;
	buttonInfo_ = static_cast<UIButtonInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIButton::setInfoButton(UIButtonInfo* _pInfo, bool _infoOwner)
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
		setVisibleState(eNormal);
		return;
	}

	state_ = eDisabled;
	setVisibleState(eDisabled);
}

void UIButton::RestoreState(State _state)
{
	if (state_ == eDisabled)
	{
		return;
	}

	if (state_ == _state)
	{
		setVisibleState(eNormal);
	}
}

void UIButton::OnMouseEnterInternalDetail(SGEventMouse* _pMouseEvent)
{
	setVisibleState(eOver);
}

void UIButton::OnMouseLeaveInternalDetail(SGEventMouse* _pMouseEvent)
{
	setVisibleState(eNormal);
}

bool UIButton::OnMouseMoveInternalDetail(SGEventMouse* _pMouseEvent)
{
	return false;
}

bool UIButton::OnMouseDownInternalDetail(SGEventMouse* _pMouseEvent)
{
	setVisibleState(ePressed);
	return false;
}

void UIButton::OnMouseUpInternalDetail(SGEventMouse* _pMouseEvent)
{
	setVisibleState(eNormal);
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

	const ImagePack* pPack = Core::Contents.PackManager->getPackUnsafe(buttonInfo_->Sga);
	SetInitialUISize(DEFAULT_SIZE30);

	if (pPack == nullptr)
	{
		_LogWarn_("버튼 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteAbstractPtr pSprite = pPack->getSpriteUnsafe(buttonInfo_->Img, buttonInfo_->Sprites[eNormal]);

	if (pSprite == nullptr)
	{
		_LogWarn_("버튼 노말 스프라이트를 찾지 못했습니다.");
		return false;
	}

	const SgaSpriteRect spriteRect = pSprite->GetRect();
	SetInitialUISize({ spriteRect.GetWidthF(), spriteRect.GetHeightF() });
	return isInitialized_ = true;
}

void UIButton::Load()
{
	if (isLoaded_)
	{
		return;
	}

	for (int i = 0; i < eMax; ++i)
	{
		const int spriteIndex = buttonInfo_->Sprites[i];

		if (spriteIndex == InvalidValue_v)
		{
			_LogWarn_("설정되지 않은 스프라이트입니다.");
			continue;
		}

		FrameTexture* pTexture = Core::Contents.UIManager->createUITextureRetained(
			buttonInfo_->Sga, buttonInfo_->Img, spriteIndex, buttonInfo_->LinearDodge);

		Sprite* pSprite = Sprite::create();
		pSprite->initWithTexture(pTexture->getTexture());
		pSprite->setScaleX(uiSize_.width / pTexture->getWidthF());
		pSprite->setScaleY(uiSize_.height / pTexture->getHeightF());
		pSprite->setAnchorPoint(Vec2::ZERO);

		texture_[i] = pTexture;
		sprite_[i] = pSprite;

		this->addChild(pSprite);
	}

	setVisibleState(eNormal);

	isLoaded_ = true;
}

void UIButton::Unload()
{
	if (!isLoaded_)
	{
		return;
	}

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < eMax; ++i)
	{
		sprite_[i] = nullptr;
		CC_SAFE_RELEASE_NULL(texture_[i]);
	}

	isLoaded_ = false;
}
