/*
 * 작성자: 윤정도
 * 생성일: 2/23/2023 12:10:35 AM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/UIElement.h>

class UICheckBox : public UIElement
{
public:
	static constexpr int TextureCount = 4;
	static constexpr int IndexBackground = 0;
	static constexpr int IndexBackgroundDisabled = 1;
	static constexpr int IndexCross = 2;
	static constexpr int IndexCrossDisabled = 3;

	static UICheckBox* create(UIRootGroup* _pMaster, UIGroup* _pParent);
	static UICheckBox* create(UIRootGroup* _pMaster, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo,
	                          bool _infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::CheckBox; }

	UICheckBox(UIRootGroup* _pMaster, UIGroup* _pParent);
	UICheckBox(UIRootGroup* _pMaster, UIGroup* _pParent, UICheckBoxInfo* _pCheckBoxInfo, bool _infoOwner);
	~UICheckBox() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void setCheck(bool _checked);
	void SetEnabled(bool _enabled) override;
	void SetUISize(const SGSize& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void setInfoCheckBox(UICheckBoxInfo* _pInfo, bool _infoOwner);
	bool isChecked() const;

	UIElementType_t GetElementType() override { return UIElementType::CheckBox; }
	SGString ToString() override { return SGStringUtil::Format("체크박스(%d)", pInfo_->code_); }

protected:
	bool OnMouseUpContainedInternalDetail(SGEventMouse* _pMouseEvent) override;

private:
	UICheckBoxInfo* pInfo_;
	FrameTexture* pTexture_[TextureCount];
	SGSprite* pSprite_[TextureCount];
	bool checked_;
};
