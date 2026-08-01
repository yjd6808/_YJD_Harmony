#pragma once

#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/UI/UIRegister.h"

class UI_Generic : public UIRootGroup
{
public:
	UI_Generic(UIGroupInfo* _pGroupInfo);

protected:
	void OnInit(const jc::CDataMap<>& _param) override;
	void OnLoaded() override;
	void OnUpdate(float _dt) override;
	void OnMouseUpTarget(UIElement* _pElement, cc::EventMouse* _pMouseEvent) override;

private:
	UILabel* pLabelTitle_;
	UIButton* pBtnClose_;
	UICheckBox* pChkA_;
	UICheckBox* pChkB_;
	UICheckBox* pChkC_;
	UICheckBox* pChkD_;
	UIToggleButton* pToggleOpt_;
	UIProgressBar* pPbar1_;
	UIProgressBar* pPbar2_;
	UIProgressBar* pPbar3_;
	UIProgressBar* pPbar4_;
	UIProgressBar* pPbar5_;
	UIProgressBar* pPbar6_;

	UIButton* pBtnThemeDark_;
	UIButton* pBtnThemeLight_;
	UIButton* pBtnThemeSilver_;
	UILabel* pLabelThemeStatus_;

	const char* pLastThemeId_ = "modern-dark-gold";

	void ApplyThemeFile(const char* _jsonName, UIColorScheme _scheme);
	void UpdateThemeStatusLabel();
	void BuildThemeTestButtons();
	UIButton* CreateTestButton(const char* _name, const char* _label, float _x, float _y, float _w, float _h);
};
