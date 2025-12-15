/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:22 PM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UISprite.h"

#include <SteinsGate/Client/UIRootGroup.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UISprite::UISprite(UIRootGroup* _pMasterGroup, UIGroup* _pParent)
: UIElement(_pMasterGroup, _pParent)
, spriteInfo_(nullptr)
, texture_(nullptr)
, sprite_(nullptr)
{
}

UISprite::UISprite(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UISpriteInfo* _pSpriteInfo, bool _infoOwner)
: UIElement(_pMasterGroup, _pParent, _pSpriteInfo, _infoOwner)
, spriteInfo_(_pSpriteInfo)
, texture_(nullptr)
, sprite_(nullptr)
{
}

UISprite::~UISprite()
{
	CC_SAFE_RELEASE(texture_);
}

UISprite* UISprite::create(UIRootGroup* _pMasterGroup, UIGroup* _pParent)
{
	UISprite* pSprite = dbg_new UISprite(_pMasterGroup, _pParent);
	pSprite->init();
	pSprite->autorelease();
	return pSprite;
}

UISprite* UISprite::create(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UISpriteInfo* _pSpriteInfo, bool _infoOwner)
{
	UISprite* pSprite = dbg_new UISprite(_pMasterGroup, _pParent, _pSpriteInfo, _infoOwner);
	pSprite->init();
	pSprite->autorelease();
	return pSprite;
}

bool UISprite::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	SGSize initialSize = spriteInfo_->Size;

	if (int(initialSize.width) == 0 || int(initialSize.width) == 0)
	{
		initialSize = Core::Contents.PackManager->getTextureSize(spriteInfo_->Sga, spriteInfo_->Img,
		                                                         spriteInfo_->Sprite);
	}

	SetInitialUISize(initialSize);
	return isInitialized_ = true;
}

void UISprite::Load()
{
	if (isLoaded_)
	{
		return;
	}

	texture_ = Core::Contents.UIManager->createUITextureRetained(spriteInfo_->Sga, spriteInfo_->Img,
	                                                             spriteInfo_->Sprite, spriteInfo_->LinearDodge);

	if (texture_->isLink())
	{
		CC_SAFE_RELEASE_NULL(texture_);
		return;
	}

	const Size spriteSize = texture_->getSize();
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

	sprite_ = spriteInfo_->Scale9 ? Scale9Sprite::create() : Sprite::create();
	sprite_->initWithTexture(texture_->getTexture());
	sprite_->setAnchorPoint(Vec2::ZERO);

	if (spriteInfo_->Scale9)
	{
		sprite_->setContentSize({ spriteSize.width * scaleX, spriteSize.height * scaleY });
	}
	else
	{
		sprite_->setScale(scaleX, scaleY);
	}

	this->addChild(sprite_);
	isLoaded_ = true;
}

void UISprite::Unload()
{
	if (!isLoaded_)
	{
		return;
	}

	removeAllChildren(); // autorelease 되기땜
	sprite_ = nullptr;
	CC_SAFE_RELEASE_NULL(texture_);
	isLoaded_ = false;
}

void UISprite::setCapInsets(const SGRect& _insets)
{
	if (!spriteInfo_->Scale9)
	{
		_LogWarn_("스케일9 스프가 아닌데 setCapInsets 시도");
		return;
	}

	if (!isLoaded_)
	{
		_LogWarn_("로딩 안됬는데 setCapInsets 시도");
		return;
	}

	dynamic_cast<Scale9Sprite*>(sprite_)->setCapInsets(_insets);
}

void UISprite::SetUISize(const SGSize& _size)
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

	if (texture_ == nullptr || sprite_ == nullptr)
	{
		return;
	}

	const Size spriteSize = texture_->getSize();
	const float scaleX = uiSize_.width / spriteSize.width;
	const float scaleY = uiSize_.height / spriteSize.height;

	if (spriteInfo_->Scale9)
	{
		sprite_->setContentSize({ spriteSize.width * scaleX, spriteSize.height * scaleY });
	}
	else
	{
		sprite_->setScale(scaleX, scaleY);
	}
}

void UISprite::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->Type != UIElementType::Sprite)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(pBaseInfo_);
	}

	pBaseInfo_ = _pInfo;
	spriteInfo_ = static_cast<UISpriteInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UISprite::setInfoSprite(UISpriteInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

void UISprite::setOpacity(GLubyte _opacity)
{
	UIElement::setOpacity(_opacity);

	if (sprite_)
	{
		sprite_->setOpacity(_opacity);
	}
}
