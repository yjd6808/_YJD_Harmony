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
	static UISprite* Create(UIRootGroup* _pMasterGroup, UIGroup* _pParent);
	static UISprite* Create(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UISpriteInfo* _pSpriteInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::Sprite; }

	UISprite(UIRootGroup* _pMasterGroup, UIGroup* _pParent);
	UISprite(UIRootGroup* _pMasterGroup, UIGroup* _pParent, UISpriteInfo* _pSpriteInfo, bool _infoOwner);
	~UISprite() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void SetCapInsets(const SGRect& _insets);
	void SetUISize(const SGSize& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoSprite(UISpriteInfo* _pInfo, bool _infoOwner);
	void setOpacity(GLubyte _opacity) override;
	bool IsScale9() const { return pInfo_->scale9_; }

	UISpriteInfo* GetInfo() const { return pInfo_; }
	SGSprite* Source() const { return pSprite_; }
	UIElementType_t GetElementType() override { return UIElementType::Sprite; }
	SGString ToString() override { return SGStringUtil::Format("스프라이트(%d)", pInfo_->code_); }

private:
	UISpriteInfo* pInfo_;
	FrameTexture* pTexture_;
	SGSprite* pSprite_;
};
