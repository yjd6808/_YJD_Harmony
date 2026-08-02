/*
 * 작성자: 윤정도
 * =====================
 * sgui - 토글 계열 컨트롤 (WPF ToggleButton / CheckBox / RadioButton)
 * 체크 상태 변경 시 ueCheckedChanged(UIEventChecked) 버블링 이벤트를 발생시킨다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/Button.h"

namespace sgui
{

class ToggleButton : public ButtonBase
{
public:
	static ToggleButton* Create(const char* _text = nullptr);

	void SetIsChecked(bool _checked);
	bool IsChecked() const { return isChecked_; }

	virtual void RefreshThemeVisuals() override;

	// 스위치 비주얼 (Track + Knob) 활성화. 외부에서 ToggleButton을 스위치로 쓸 때 호출한다.
	// CheckBox/RadioButton은 자체 비주얼로 대체하므로 호출하지 않는다.
	void BuildSwitchVisuals();

protected:
	virtual void OnClick() override;
	virtual void OnCheckedChanged();

	virtual UIVisualState MapVisualState() const override;
	virtual void OnRenderSizeChanged(const cc::size& _size) override;

	bool isChecked_ = false;
	bool useSwitchVisuals_ = false;

	BrushVisual trackVisual_;
	BrushVisual knobVisual_;
};

class CheckBox : public ToggleButton
{
public:
	static CheckBox* Create(const char* _text = nullptr);

	virtual void RefreshThemeVisuals() override;

protected:
	virtual void OnCheckedChanged() override;
	virtual void OnRenderSizeChanged(const cc::size& _size) override;
	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;

	static constexpr float BoxSize_v = 18.0f;
	static constexpr float BoxGap_v = 6.0f;

	BrushVisual boxVisual_;
	BrushVisual markVisual_;
};

class RadioButton : public ToggleButton
{
public:
	static RadioButton* Create(const char* _text = nullptr, const char* _groupName = "");

	void SetGroupName(const char* _groupName) { groupName_ = _groupName ? _groupName : ""; }
	const std::string& GetGroupName() const { return groupName_; }

	virtual void RefreshThemeVisuals() override;

protected:
	virtual void OnClick() override;
	virtual void OnCheckedChanged() override;
	virtual void OnRenderSizeChanged(const cc::size& _size) override;
	virtual cc::size MeasureOverride(const cc::size& _constraint) override;
	virtual void ArrangeOverride(const cc::size& _finalSize) override;

	void UncheckGroupSiblings();

	static constexpr float RadioSize_v = 18.0f;
	static constexpr float RadioGap_v = 6.0f;

	std::string groupName_;

	BrushVisual radioVisual_;
	BrushVisual dotVisual_;
};

} // namespace sgui
