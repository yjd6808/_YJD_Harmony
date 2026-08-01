/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 4:40:33 PM
 * =====================
 *
 */


#pragma once


#include "sgcl/Game/UI/UIElement.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIThemeBinding.h"

class UIStatic : public UIElement
{
public:
	static UIStatic* Create(UIRootGroup* _pRoot, UIGroup* _pParent);
	static UIStatic* Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::Static; }

	UIStatic(UIRootGroup* _pRoot, UIGroup* _pParent);
	UIStatic(UIRootGroup* _pRoot, UIGroup* _pParent, UIStaticInfo* _pStaticInfo, bool _infoOwner);
	~UIStatic() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void SetDebugVisible(bool _visible);
	void SetUISize(const cc::size& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoStatic(UIStaticInfo* _pInfo, bool _infoOwner);

	UIStaticInfo* GetInfo() const { return pInfo_; }
	UIElementType_t GetElementType() override { return UIElementType::Static; }
	jc::String ToString() override { return jc::StringUtil::Format("스태틱(%s)", pInfo_->name_); }

	UITextureMode GetTextureMode() const { return textureMode_; }
	void RefreshThemeVisuals();

protected:
	void BuildThemeVisuals();

private:
	bool visible_;
	UIStaticInfo* pInfo_;
	cc::Sprite* pDebugSprite_;
	cc_ui::Scale9Sprite* themeRoot_ = nullptr;
	UITextureMode textureMode_ = UITextureMode::NONE;
	UIThemeTextureBinding themeBinding_;
};
