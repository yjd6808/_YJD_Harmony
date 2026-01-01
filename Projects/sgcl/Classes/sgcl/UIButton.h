/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 5:18:04 AM
 * =====================
 *
 */


#pragma once

#include <sgcl/UIElement.h>

class UIButton : public UIElement
{
public:
	static UIButton* Create(UIRootGroup* _pRoot, UIGroup* _pParent);
	static UIButton* Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIButtonInfo* _pButtonInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::Button; }

	UIButton(UIRootGroup* _pRoot, UIGroup* _pParent);
	UIButton(UIRootGroup* _pRoot, UIGroup* _pParent, UIButtonInfo* _pButtonInfo, bool _infoOwner);
	~UIButton() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void SetVisibleState(State _state);
	void SetUISize(const c2d::size& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoButton(UIButtonInfo* _pInfo, bool _infoOwner);
	void SetEnabled(bool _enabled) override;
	void RestoreState(State _state) override;
	UIElementType_t GetElementType() override { return UIElementType::Button; }
	jc::String ToString() override { return jc::StringUtil::Format("버튼(%d)", buttonInfo_->code_); }

protected:
	void OnMouseEnterInternalDetail(c2d::EventMouse* _pMouseEvent) override;
	void OnMouseLeaveInternalDetail(c2d::EventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(c2d::EventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(c2d::EventMouse* _pMouseEvent) override;
	void OnMouseUpInternalDetail(c2d::EventMouse* _pMouseEvent) override;

private:
	UIButtonInfo* buttonInfo_;
	FrameTexture* texture_[eMax];
	c2d::Sprite* sprite_[eMax];
};
