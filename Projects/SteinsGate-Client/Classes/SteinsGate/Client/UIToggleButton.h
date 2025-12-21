/*
 * 작성자: 윤정도
 * 생성일: 3/21/2023 1:18:17 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/UIElement.h>


enum class ToggleState
{
	eNormal,
	eToggled
};


class UIToggleButton : public UIElement
{
	static constexpr int STATE_ONE = 0;
	static constexpr int STATE_TWO = 1;

public:
	static UIToggleButton* Create(UIRootGroup* _pRoot, UIGroup* _pParent);
	static UIToggleButton* Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIToggleButtonInfo* _pBtnInfo, bool _infoOwner);
	static constexpr UIElementType_t Type() { return UIElementType::ToggleButton; }

	UIToggleButton(UIRootGroup* _pRoot, UIGroup* _pParent);
	UIToggleButton(UIRootGroup* _pRoot, UIGroup* _pParent, UIToggleButtonInfo* _pBtnInfo, bool _infoOwner);
	~UIToggleButton() override;

	bool init() override;
	void Load() override;
	void Unload() override;

	void SetVisibleState(State _state);
	void SetEnabled(bool _enabled) override;
	void SetUISize(const SGSize& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoToggleButton(UIToggleButtonInfo* _pInfo, bool _infoOwner);

	ToggleState GetToggleState() const { return toggleState_; }
	void SetToggleState(ToggleState _state);
	void RestoreState(State _state) override;

	UIElementType_t GetElementType() override { return UIElementType::ToggleButton; }
	SGString ToString() override { return SGStringUtil::Format("토글버튼(%d)", pInfo_->code_); }

protected:
	void OnMouseEnterInternalDetail(SGEventMouse* _pMouseEvent) override;
	void OnMouseLeaveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(SGEventMouse* _pMouseEvent) override;
	void OnMouseUpInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseUpContainedInternalDetail(SGEventMouse* _pMouseEvent) override;

private:
	ToggleState toggleState_;
	UIToggleButtonInfo* pInfo_;
	FrameTexture* pTextures_[2][eMax];
	SGSprite* pSprites_[2][eMax];
};
