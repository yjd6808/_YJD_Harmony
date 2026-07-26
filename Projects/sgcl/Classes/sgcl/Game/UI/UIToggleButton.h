#pragma once

#include "sgcl/Game/UI/UIElement.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIThemeBinding.h"

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
	void SetUISize(const cc::size& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoToggleButton(UIToggleButtonInfo* _pInfo, bool _infoOwner);

	ToggleState GetToggleState() const { return toggleState_; }
	void SetToggleState(ToggleState _state);
	void RestoreState(State _state) override;

	void SetRenderMode(UIRenderMode _mode) { renderMode_ = _mode; }
	bool UseThemeRendering() const { return renderMode_ == UIRenderMode::Theme; }
	void RefreshThemeVisuals();

	UIElementType_t GetElementType() override { return UIElementType::ToggleButton; }
	jc::String ToString() override { return jc::StringUtil::Format("토글버튼(%s)", pInfo_->name_); }

protected:
	void OnMouseEnterInternalDetail(cc::EventMouse* _pMouseEvent) override;
	void OnMouseLeaveInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(cc::EventMouse* _pMouseEvent) override;
	void OnMouseUpInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseUpContainedInternalDetail(cc::EventMouse* _pMouseEvent) override;

	void LoadTheme();
	bool LoadLegacy();
	void BuildThemeVisuals();
	void DestroyThemeVisuals();
	void ApplyThemeStateVisuals(State _state);

private:
	ToggleState toggleState_;
	UIToggleButtonInfo* pInfo_;
	FrameTexture* pTextures_[2][eMax];
	cc::Sprite* pSprites_[2][eMax];

	UIRenderMode renderMode_ = UIRenderMode::Auto;
	UIThemeTextureBinding themeBinding_;
	uint64_t appliedTextureRevision_ = 0;
	cc::Node* themeTrack_ = nullptr;
	cc::Sprite* themeKnob_ = nullptr;
};
