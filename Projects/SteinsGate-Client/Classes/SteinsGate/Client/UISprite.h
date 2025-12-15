/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:15 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIElement.h>

class UISprite : public UIElement
{
public:
	static UISprite* create(UIRootGroup* _pMasterGroup, UIGroup* _pParent);
	static UISprite* create(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UISpriteInfo* _pSpriteInfo,
	                        bool _infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::Sprite; }

	UISprite(UIRootGroup* _pMasterGroup, UIGroup* _pParent);
	UISprite(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UISpriteInfo* _pSpriteInfo, bool _infoOwner);
	~UISprite() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void setCapInsets(const SGRect& _insets);
	void SetUISize(const SGSize& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void setInfoSprite(UISpriteInfo* _pInfo, bool _infoOwner);
	void setOpacity(GLubyte _opacity) override;
	bool isScale9() const { return spriteInfo_->Scale9; }

	UISpriteInfo* getInfo() const { return spriteInfo_; }
	SGSprite* source() const { return sprite_; }
	UIElementType_t GetElementType() override { return UIElementType::Sprite; }
	SGString ToString() override { return SGStringUtil::Format("스프라이트(%d)", spriteInfo_->code_); }

private:
	UISpriteInfo* spriteInfo_;
	FrameTexture* texture_;
	SGSprite* sprite_;
};
