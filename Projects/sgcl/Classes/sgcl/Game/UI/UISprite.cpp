/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:22 PM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "Game/UI/UISprite.h"

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/Texture/ImagePackManager.h"

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UISprite::UISprite(UIRootGroup* _pMasterGroup, UIGroup* _pParent)
: UIElement(_pMasterGroup, _pParent)
, pInfo_(nullptr)
, pTexture_(nullptr)
, pSprite_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UISprite::UISprite(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UISpriteInfo* _pSpriteInfo, bool _infoOwner)
: UIElement(_pMasterGroup, _pParent, _pSpriteInfo, _infoOwner)
, pInfo_(_pSpriteInfo)
, pTexture_(nullptr)
, pSprite_(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UISprite::~UISprite()
{
	CC_SAFE_RELEASE(pTexture_);
}

//////////////////////////////////////////////////////////////////////////////////////////
UISprite* UISprite::Create(UIRootGroup* _pMasterGroup, UIGroup* _pParent)
{
	UISprite* pSprite = dbg_new UISprite(_pMasterGroup, _pParent);
	pSprite->init();
	pSprite->autorelease();
	return pSprite;
}

//////////////////////////////////////////////////////////////////////////////////////////
UISprite* UISprite::Create(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UISpriteInfo* _pSpriteInfo, bool _infoOwner)
{
	UISprite* pSprite = dbg_new UISprite(_pMasterGroup, _pParent, _pSpriteInfo, _infoOwner);
	pSprite->init();
	pSprite->autorelease();
	return pSprite;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UISprite::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	cc::size initialSize = pInfo_->size_;

	if (int(initialSize.width) == 0 || int(initialSize.height) == 0)
	{
		initialSize = g_cImagePackMgr.GetTextureSize(
			pInfo_->sga_, pInfo_->img_, pInfo_->sprite_);
	}

	SetInitialUISize(initialSize);
	return isInitialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UISprite::Load()
{
	if (isLoaded_)
	{
		return;
	}

	pTexture_ = g_cUIMgr.CreateUITextureRetained(pInfo_->sga_, pInfo_->img_, pInfo_->sprite_, pInfo_->linearDodge_);

	if (pTexture_->IsLink())
	{
		CC_SAFE_RELEASE_NULL(pTexture_);
		return;
	}

	const Size spriteSize = pTexture_->GetSize();
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	if (uiSize_.width > 0.0f)
	{
		scaleX = uiSize_.width / spriteSize.width;
	}

	if (uiSize_.height > 0.0f)
	{
		scaleY = uiSize_.height / spriteSize.height;
	}

	pSprite_ = pInfo_->scale9_ ? Scale9Sprite::create() : Sprite::create();
	pSprite_->initWithTexture(pTexture_->GetTexture());
	pSprite_->setAnchorPoint(Vec2::ZERO);

	if (pInfo_->scale9_)
	{
		pSprite_->setContentSize({ spriteSize.width * scaleX, spriteSize.height * scaleY });
	}
	else
	{
		pSprite_->setScale(scaleX, scaleY);
	}

	this->addChild(pSprite_);
	isLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UISprite::Unload()
{
	if (!isLoaded_)
	{
		return;
	}

	removeAllChildren(); // autorelease 되기땜
	pSprite_ = nullptr;
	CC_SAFE_RELEASE_NULL(pTexture_);
	isLoaded_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UISprite::SetCapInsets(const cc::rect& _insets)
{
	if (!pInfo_->scale9_)
	{
		_LogWarn_("스케일9 스프가 아닌데 setCapInsets 시도");
		return;
	}

	if (!isLoaded_)
	{
		_LogWarn_("로딩 안됬는데 setCapInsets 시도");
		return;
	}

	dynamic_cast<Scale9Sprite*>(pSprite_)->setCapInsets(_insets);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UISprite::SetUISize(const cc::size& _size)
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

	if (pTexture_ == nullptr || pSprite_ == nullptr)
	{
		return;
	}

	const Size spriteSize = pTexture_->GetSize();
	const float scaleX = uiSize_.width / spriteSize.width;
	const float scaleY = uiSize_.height / spriteSize.height;

	if (pInfo_->scale9_)
	{
		pSprite_->setContentSize({ spriteSize.width * scaleX, spriteSize.height * scaleY });
	}
	else
	{
		pSprite_->setScale(scaleX, scaleY);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UISprite::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::Sprite)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JC_DELETE_SAFE(pBaseInfo_);
	}

	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UISpriteInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UISprite::SetInfoSprite(UISpriteInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UISprite::setOpacity(GLubyte _opacity)
{
	UIElement::setOpacity(_opacity);

	if (pSprite_)
	{
		pSprite_->setOpacity(_opacity);
	}
}
