/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:43 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIStatic.h"

#include <SteinsGate/Client/UIRootGroup.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::UIStatic(UIRootGroup* _pMaster, UIGroup* _pParent)
: UIElement(_pMaster, _pParent)
, visible_(false)
, info_(nullptr)
, debugTexture_{}
, debugSprite_{}
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::UIStatic(UIRootGroup* _pMaster, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner)
: UIElement(_pMaster, _pParent, _pStaticInfo, _infoOwner)
, visible_(false)
, info_(_pStaticInfo)
, debugTexture_{}
, debugSprite_{}
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::~UIStatic()
{
	CC_SAFE_RELEASE(debugTexture_);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic* UIStatic::create(UIRootGroup* _pMaster, UIGroup* _pParent)
{
	UIStatic* pStatic = dbg_new UIStatic(_pMaster, _pParent);
	pStatic->init();
	pStatic->autorelease();
	return pStatic;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic* UIStatic::create(UIRootGroup* _pMaster, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner)
{
	UIStatic* pStatic = dbg_new UIStatic(_pMaster, _pParent, _pStaticInfo, _infoOwner);
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

	if (info_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	SetInitialUISize(info_->Size);
	return isInitialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::Load()
{
	if (isLoaded_)
		return;

	debugTexture_ = Core::Contents.Global->getDefaultFrameTexture();
	debugTexture_->retain();

	const Size spriteSize = debugTexture_->getSize();
	const float scaleX = uiSize_.width / spriteSize.width;
	const float scaleY = uiSize_.height / spriteSize.height;

	debugSprite_ = Sprite::create();
	debugSprite_->initWithTexture(debugTexture_->getTexture());
	debugSprite_->setAnchorPoint(Vec2::ZERO);
	debugSprite_->setScale(scaleX, scaleY);
	debugSprite_->setOpacity(125);
	debugSprite_->setColor(SGColorList::Africanviolet_v);
	debugSprite_->setVisible(visible_);

	this->addChild(debugSprite_);
	isLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::Unload()
{
	if (isLoaded_ == false)
		return;

	removeAllChildren(); // autorelease 되기땜
	debugSprite_ = nullptr;
	CC_SAFE_RELEASE_NULL(debugTexture_);
	isLoaded_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::setDebugVisible(bool _visible)
{
	visible_ = _visible;

	if (debugSprite_)
		debugSprite_->setVisible(visible_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::SetUISize(const SGSize& _contentSize)
{
	if (!isResizable_)
		return;

	UIElement::setContentSize(_contentSize);

	if (!isLoaded_)
		return;

	debugTexture_ = Core::Contents.Global->getDefaultFrameTexture();

	const Size spriteSize = debugTexture_->getSize();
	const float scaleX = uiSize_.width / spriteSize.width;
	const float scaleY = uiSize_.height / spriteSize.height;

	debugSprite_->setScale(scaleX, scaleY);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->Type != UIElementType::Static)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(info_);
	}

	pBaseInfo_ = _pInfo;
	info_ = static_cast<UIStaticInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::setInfoStatic(UIStaticInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}
