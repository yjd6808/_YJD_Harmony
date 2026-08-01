/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:43 PM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "Game/UI/UIStatic.h"

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::UIStatic(UIRootGroup* _pRoot, UIGroup* _pParent)
: UIElement(_pRoot, _pParent)
, visible_(false)
, pInfo_(nullptr)
, pDebugSprite_{}
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::UIStatic(UIRootGroup* _pRoot, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner)
: UIElement(_pRoot, _pParent, _pStaticInfo, _infoOwner)
, visible_(false)
, pInfo_(_pStaticInfo)
, pDebugSprite_{}
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIStatic::~UIStatic()
{
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

	textureMode_ = UITextureMode::THEME;
	BuildThemeVisuals();
	isLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::BuildThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();

	UIResolvedStyle resolved = pThemeMgr->Resolve(UIElementType::Group, UIVisualState::Normal, {});
	uint64_t hash = resolved.ComputeHash();

	auto* pFrame = Scale9Sprite::create();
	pFrame->setAnchorPoint(Vec2::ZERO);
	pFrame->setContentSize(uiSize_);
	pFrame->setOpacity(125);
	pFrame->setColor(ColorList::Africanviolet_v);
	pFrame->setVisible(visible_);
	this->addChild(pFrame);

	pDebugSprite_ = pFrame;
	themeRoot_ = pFrame;

	UIAssetKey key = UIAssetKey::For(UIAssetSemantic::Frame, hash);
	themeBinding_.BindScale9(pFrame, key, UIComponentSlot::Background);

	_LogDebug_("[UIStatic] BuildThemeVisuals styleHash=%llu name=%s size=(%.0f,%.0f)",
		hash, GetName(), uiSize_.width, uiSize_.height);

	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);
	else
		_LogWarn_("[UIStatic] GetActiveTextureSet returned null!");

	// setSpriteFrame가 contentSize를 리셋하므로 복구한다.
	pFrame->setContentSize(uiSize_);
	pFrame->setVisible(visible_);

	_LogDebug_("[UIStatic] BuildThemeVisuals final: name=%s contentSize=(%.0f,%.0f) visible=%d",
		GetName(), pFrame->getContentSize().width, pFrame->getContentSize().height, pFrame->isVisible());
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::Unload()
{
	if (isLoaded_ == false)
		return;

	removeAllChildren(); // autorelease 되기땜
	pDebugSprite_ = nullptr;
	themeRoot_ = nullptr;
	themeBinding_.Clear();
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
void UIStatic::SetUISize(const cc::size& _contentSize)
{
	if (!isResizable_)
		return;

	UIElement::setContentSize(_contentSize);

	if (!isLoaded_)
		return;

	if (textureMode_ == UITextureMode::THEME)
	{
		if (themeRoot_)
			themeRoot_->setContentSize(_contentSize);
		return;
	}
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
		JC_DELETE_SAFE(pInfo_);
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

//////////////////////////////////////////////////////////////////////////////////////////
void UIStatic::RefreshThemeVisuals()
{
	UIThemeManager* pThemeMgr = UIThemeManager::Get();
	const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet();
	if (pSet)
		themeBinding_.Refresh(*pSet);

	// setSpriteFrame가 contentSize를 리셋하므로 복구한다.
	if (themeRoot_)
		themeRoot_->setContentSize(uiSize_);
}
