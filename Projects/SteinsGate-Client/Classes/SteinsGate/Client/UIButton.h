/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 5:18:04 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIElement.h>

class UIButton : public UIElement
{
public:
	static UIButton* create(UIRootGroup* _pMaster, UIGroup* _pParent);
	static UIButton* create(UIRootGroup* _pMaster, UIGroup* _pParent, UIButtonInfo* _pButtonInfo, bool _infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::Button; }

	UIButton(UIRootGroup* _pMaster, UIGroup* _pParent);
	UIButton(UIRootGroup* _pMaster, UIGroup* _pParent, UIButtonInfo* _pButtonInfo, bool _infoOwner);
	~UIButton() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void setVisibleState(State _state);
	void SetUISize(const SGSize& _size) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void setInfoButton(UIButtonInfo* _pInfo, bool _infoOwner);
	void SetEnabled(bool _enabled) override;
	void RestoreState(State _state) override;
	UIElementType_t GetElementType() override { return UIElementType::Button; }
	SGString ToString() override { return SGStringUtil::Format("버튼(%d)", buttonInfo_->code_); }

protected:
	void OnMouseEnterInternalDetail(SGEventMouse* _pMouseEvent) override;
	void OnMouseLeaveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(SGEventMouse* _pMouseEvent) override;
	void OnMouseUpInternalDetail(SGEventMouse* _pMouseEvent) override;

private:
	UIButtonInfo* buttonInfo_;
	FrameTexture* texture_[eMax];
	SGSprite* sprite_[eMax];
};
