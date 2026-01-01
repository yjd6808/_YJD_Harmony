/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:43 PM
 * =====================
 *
 */


#include "Core.h"
#include "GameCoreHeader.h"
#include "UIStatic.h"

#include <sgcl/UIRootGroup.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::UIStatic(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, visible_(false)
, pInfo_(nullptr)
, pDebugTexture_{}
, pDebugSprite_{}
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::UIStatic(UIRootGroup* _pRoot, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner)
: UIElement(_pRoot, _pParent, _pStaticInfo, _infoOwner)
, visible_(false)
, pInfo_(_pStaticInfo)
, pDebugTexture_{}
, pDebugSprite_{}
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::~UIStatic()
{
	CC_SAFE_RELEASE(pDebugTexture_);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic* UIStatic::Create(UIRootGroup* _pRoot, UIGroup* _pParent)
{
	UIStatic* pStatic = dbg_new UIStatic(_pRoot, _pParent);
	pStatic->init();
	pStatic->autorelease();
	return pStatic;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic* UIStatic::Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner)
{
	UIStatic* pStatic = dbg_new UIStatic(_pRoot, _pParent, _pStaticInfo, _infoOwner);
	pStatic->init();
	pStatic->autorelease();
	return pStatic;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIStatic::init()
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

	SetInitialUISize(pInfo_->Size);
	return isInitialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::Load()
{
	if (isLoaded_)
		return;

	pDebugTexture_ = sg::Contents.Global->getDefaultFrameTexture();
	pDebugTexture_->retain();

	const Size spriteSize = pDebugTexture_->GetSize();
	const float scaleX = uiSize_.width / spriteSize.width;
	const float scaleY = uiSize_.height / spriteSize.height;

	pDebugSprite_ = Sprite::create();
	pDebugSprite_->initWithTexture(pDebugTexture_->GetTexture());
	pDebugSprite_->setAnchorPoint(Vec2::ZERO);
	pDebugSprite_->setScale(scaleX, scaleY);
	pDebugSprite_->setOpacity(125);
	pDebugSprite_->setColor(ColorList::Africanviolet_v);
	pDebugSprite_->setVisible(visible_);

	this->addChild(pDebugSprite_);
	isLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::Unload()
{
	if (isLoaded_ == false)
		return;

	removeAllChildren(); // autorelease 되기땜
	pDebugSprite_ = nullptr;
	CC_SAFE_RELEASE_NULL(pDebugTexture_);
	isLoaded_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::SetDebugVisible(bool _visible)
{
	visible_ = _visible;

	if (pDebugSprite_)
		pDebugSprite_->setVisible(visible_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::SetUISize(const c2d::size& _contentSize)
{
	if (!isResizable_)
		return;

	UIElement::setContentSize(_contentSize);

	if (!isLoaded_)
		return;

	pDebugTexture_ = sg::Contents.Global->getDefaultFrameTexture();

	const Size spriteSize = pDebugTexture_->GetSize();
	const float scaleX = uiSize_.width / spriteSize.width;
	const float scaleY = uiSize_.height / spriteSize.height;

	pDebugSprite_->setScale(scaleX, scaleY);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::Static)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(pInfo_);
	}

	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UIStaticInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::SetInfoStatic(UIStaticInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}
