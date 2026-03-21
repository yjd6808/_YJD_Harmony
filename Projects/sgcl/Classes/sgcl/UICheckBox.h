/*
 * 작성자: 윤정도
 * 생성일: 2/23/2023 12:10:35 AM
 * =====================
 *
 */

#pragma once

#include "sgcl/UIElement.h"

class UICheckBox : public UIElement
{
public:
	static constexpr int TEXTURE_COUNT = 4;
	static constexpr int INDEX_BACKGROUND = 0;
	static constexpr int INDEX_BACKGROUND_DISABLED = 1;
	static constexpr int INDEX_CROSS = 2;
	static constexpr int INDEX_CROSS_DISABLED = 3;

	static UICheckBox* Create(UIRootGroup* _pRoot, UIGroup* _pParent);
	static UICheckBox* Create(UIRootGroup* _pRoot, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::CheckBox; }

	UICheckBox(UIRootGroup* _pRoot, UIGroup* _pParent);
	UICheckBox(UIRootGroup* _pRoot, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo, bool _infoOwner);
	~UICheckBox() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void SetCheck(bool _checked);
	void SetEnabled(bool _enabled) override;
	void SetUISize(const cc::size& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoCheckBox(UICheckBoxInfo* _pInfo, bool _infoOwner);
	bool IsChecked() const;

	UIElementType_t GetElementType() override { return UIElementType::CheckBox; }
	jc::String ToString() override { return jc::StringUtil::Format("체크박스(%d)", pInfo_->code_); }

protected:
	bool OnMouseUpContainedInternalDetail(cc::EventMouse* _pMouseEvent) override;

private:
	UICheckBoxInfo* pInfo_;
	FrameTexture* pTexture_[TEXTURE_COUNT];
	cc::Sprite* pSprite_[TEXTURE_COUNT];
	bool checked_;
};
